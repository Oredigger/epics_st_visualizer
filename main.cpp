#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

typedef struct seq_state seq_state_t;
typedef struct seq_transit seq_transit_t;

struct seq_transit
{
    std::string cond;
    std::string src_state;
    std::string dest_state;
};

enum State
{
    GLOBAL_DECLARATIONS,
    STATE_MACHINE_FOUND,
    INSIDE_STATE_MACHINE,
    STATE_FOUND,
    INSIDE_STATE_BODY,
    TRANSIT_FOUND,
    INSIDE_TRANSIT_BODY,
    END
};

std::string op = " \t\n{}()=+-/*,;.[]";

int main(void)
{
    std::ifstream fin("seq.st");
    std::stringstream ss;

    ss << fin.rdbuf();
    std::string raw_txt = ss.str();

    std::replace(raw_txt.begin(), raw_txt.end(), '\n', ' ');

    std::string token, cond, src_state;
    size_t id = 0;

    std::vector <std::string> stn;
    std::vector <seq_transit_t> tt;

    State s = GLOBAL_DECLARATIONS;
    char prev_c = ' ';
    bool waive_flag = false, is_comment_flag = false;
    bool in_state_machine_flag = false, in_state_body_flag = false, in_transit_body_flag = false;

    for (auto c : raw_txt)
    {
        if (!waive_flag && std::find(op.begin(), op.end(), c) == op.end()) 
        {
            token += c;
        }
        else
        {   
            if (c == '*' && prev_c == '/')
            {
                is_comment_flag = true;
            }
            else if (c == '/' && prev_c == '*')
            {
                is_comment_flag = false;
            }

            if (!is_comment_flag)
            {
                if (s == STATE_FOUND)
                {
                    if (in_state_body_flag)
                    {
                        tt.push_back({.cond = cond, .src_state = src_state, .dest_state = token});
                        cond.clear();

                        s = INSIDE_STATE_BODY;
                    }
                    else
                    {
                        if (std::find(stn.begin(), stn.end(), token) == stn.end())
                        {
                            stn.push_back(token);
                            src_state = token;
                        }

                        s = INSIDE_STATE_MACHINE;
                    }
                }
                else if (s == INSIDE_STATE_BODY)
                {
                    if (token == "when")
                    {
                        s = TRANSIT_FOUND;
                    }
                    else if (token == "state")
                    {
                        s = STATE_FOUND;
                    }
                    else if (c == '{')
                    {
                        s = INSIDE_TRANSIT_BODY;
                        in_transit_body_flag = true;
                    }
                    else if (c == '}' && in_state_body_flag)
                    {
                        src_state.clear();
                        s = INSIDE_STATE_MACHINE;
                        in_state_body_flag = false;
                    }
                }
                else if (s == TRANSIT_FOUND)
                {
                    if (c == '(')
                    {
                        waive_flag = true;
                    }
                    else if (c == ')')
                    {
                        waive_flag = false;
                        cond = token;
                        s = INSIDE_STATE_BODY;
                    }
                    else if (waive_flag)
                    {
                        token += c;
                    }
                }
                else if (s == INSIDE_TRANSIT_BODY)
                {
                    if (c == '}' && in_transit_body_flag)
                    {
                        in_transit_body_flag = false;
                        s = INSIDE_STATE_BODY;
                    }
                }
                else if (s == STATE_MACHINE_FOUND)
                {
                    if (c == '{')
                    {
                        s = INSIDE_STATE_MACHINE;
                        in_state_machine_flag = true;
                    }
                }
                else if (s == INSIDE_STATE_MACHINE)
                {
                    if (token == "state")
                    {
                        s = STATE_FOUND;
                    }
                    else if (c == '{')
                    {
                        s = INSIDE_STATE_BODY;
                        in_state_body_flag = true;
                    }
                    else if (c == '}' && in_state_machine_flag)
                    {
                        s = END;
                        in_state_machine_flag = false;
                    }
                }
                else if (s == GLOBAL_DECLARATIONS)
                {
                    if (token == "ss")
                    {
                        s = STATE_MACHINE_FOUND;
                    }
                }

                if (!waive_flag)
                {
                    token.clear();
                }
            }
        }

        prev_c = c;
    }    

    fin.close();
    return 0;
}

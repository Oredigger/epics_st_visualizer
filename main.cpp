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

const std::string OP = " \t\n{}()=+-/*,;.[]&|^";

int main(void)
{
    std::ifstream fin("seq.st");

    if (!fin.is_open())
    {
        std::cout << "Could not find file\n";
        return EXIT_FAILURE;
    }

    std::stringstream ss;

    ss << fin.rdbuf();
    std::string raw_txt = ss.str();

    std::replace(raw_txt.begin(), raw_txt.end(), '\n', ' ');

    std::string token, cond, src_state;
    size_t id = 0;

    std::vector <std::string> stn;
    std::vector <seq_transit_t> tt;

    int  curly_nested = 0, parenthesis_nested = 0;
    bool waive_flag = false, is_comment_flag = false;
    bool in_state_machine_flag = false, in_state_body_flag = false, in_transit_body_flag = false;

    State s = GLOBAL_DECLARATIONS;

    for (int i = 1; i < raw_txt.length(); i++)
    {
        char curr_c = raw_txt[i - 1];
        char next_c = raw_txt[i];  

        if (next_c == '*' && curr_c == '/')
        {
            is_comment_flag = true;
        }
        else if (next_c == '/' && curr_c == '*')
        {
            is_comment_flag = false;
        }
        else if (!is_comment_flag)
        {
            if (!waive_flag && std::find(OP.begin(), OP.end(), curr_c) == OP.end()) 
            {
                token += curr_c;
            }
            
            if (s == STATE_FOUND)
            {
                if (in_state_body_flag && isspace(next_c))
                {
                    tt.push_back({.cond = cond, .src_state = src_state, .dest_state = token});
                    cond.clear();
                    token.clear();
                    
                    s = INSIDE_STATE_BODY;
                }
                else if (next_c == '{')
                {
                    if (std::find(stn.begin(), stn.end(), token) == stn.end())
                    {
                        stn.push_back(token);
                        src_state = token;
                        token.clear();
                    }

                    s = INSIDE_STATE_BODY;
                    in_state_body_flag = true;
                }
            }
            else if (s == INSIDE_STATE_BODY)
            {
                if (token == "when" && next_c == '(')
                {
                    s = TRANSIT_FOUND;
                    waive_flag = true;
                    token.clear();
                }
                else if (token == "state" && isspace(next_c))
                {
                    s = STATE_FOUND;
                    token.clear();
                }
                else if (next_c == '{')
                {
                    s = INSIDE_TRANSIT_BODY;
                    in_transit_body_flag = true;
                }
                else if (next_c == '}' && in_state_body_flag)
                {
                    src_state.clear();
                    s = INSIDE_STATE_MACHINE;
                    in_state_body_flag = false;
                }
            }
            else if (s == TRANSIT_FOUND)
            {
                if (next_c == '(')
                {
                    parenthesis_nested++;
                }
                else
                {
                    if (next_c == ')')
                    {
                        if (!parenthesis_nested)
                        {
                            waive_flag = false;
                            cond = token;
                            s = INSIDE_STATE_BODY;
                        }
                        else
                        {
                            parenthesis_nested--;
                        }
                    }
                }

                token += next_c;
            }
            else if (s == INSIDE_TRANSIT_BODY)
            {
                if (in_transit_body_flag)
                {
                    if (next_c == '{')
                    {
                        curly_nested++;
                    }
                    if (next_c == '}')
                    {
                        if (!curly_nested)
                        {
                            token.clear();
                            in_transit_body_flag = false;
                            s = INSIDE_STATE_BODY;
                        }
                        else
                        {
                            curly_nested--;
                        }
                    }
                }
            }
            else if (s == STATE_MACHINE_FOUND)
            {
                if (next_c == '{')
                {
                    s = INSIDE_STATE_MACHINE;
                    in_state_machine_flag = true;
                    token.clear();
                }
            }
            else if (s == INSIDE_STATE_MACHINE)
            {
                if (token == "state" && isspace(next_c))
                {
                    s = STATE_FOUND;
                    token.clear();
                }
                else if (next_c == '}' && in_state_machine_flag)
                {
                    s = END;
                    in_state_machine_flag = false;
                }
            }
            else if (s == GLOBAL_DECLARATIONS)
            {
                if (isspace(next_c))
                {
                    if (token == "ss")
                    { 
                        s = STATE_MACHINE_FOUND;
                    }

                    token.clear();
                }
            }
        }
    }    

    std::ofstream fout;
    fout.open("out.dot", std::ofstream::trunc);

    if (fout.is_open())
    {
        fout << "digraph R{\n";

        for (auto t : tt)
        {
            fout << t.src_state << " -> " << t.dest_state << " [label=\"" << t.cond << "\" fontsize=\"8\"];\n";
        }

        fout << "}\n";
    }
    
    fout.close();
    fin.close();

    return 0;
}

/* Demo sequence.
 *
 * Run this to create dot file:
 *
 *   perl snl_dot.pl Test.st
 */
program test

double v;

ss test /* Test State Set */
{
    state start /* Start */
    {
        when (v > 5.0) {} state high /* when value is high */
        when (v < 5.0) {} state low /* when value is low */
    }state h /* Low:\nclose valve */
    {
        when (v > 5.0)
        {
        }
        state high /* when^ value is high */}

    state high /* High:\nopen valve */{when (v < 5.0)}state low /* when^ value is low */}}


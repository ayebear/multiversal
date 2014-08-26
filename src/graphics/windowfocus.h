#ifndef WINDOWFOCUS_H
#define WINDOWFOCUS_H

/*
Note: This is a temporary hack until this functionality gets included in SFML.
*/
class WindowFocus
{
    public:
        static bool hasFocus();
        static void setFocus(bool state);

    private:
        static bool focus;
};

#endif

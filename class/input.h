#ifndef STRUCT_INPUT_FRAMEWORK_H
#define STRUCT_INPUT_FRAMEWORK_H

//This still strikes me as odd. The kernel implementation shares it with the app...

namespace app
{

struct input_app
{
	enum inputs{
escape=0,
up,
down,
left,
right,
console_newline
};

};

}

#endif

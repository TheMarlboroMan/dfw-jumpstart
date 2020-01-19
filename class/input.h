#pragma once
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
activate,
console_newline,
console_backspace,
#ifdef WDEBUG_CODE
reload_debug_config
#endif
};

};

}

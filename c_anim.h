#ifndef C_ANIM_H
#define C_ANIM_H
#include <iostream>
#include <string>


#include <stdint.h>

#include "f_sdl.h"

#define N_OBJSMAX 256
#define N_SPRSMAX 256

#define N_OBJVARSMAX 16
#define N_ALARMSMAX 16

typedef void (*c_ObjectCall)(void*);
typedef uint32_t c_Var;
typedef uint8_t* c_Data;
typedef uint8_t* c_String;

typedef enum {
	c_ObjX = 0,
	c_ObjY = 1,
	c_ObjSprite = 2,
	c_ObjSpriteIndex = 3,
} c_ObjVar;

typedef struct {
	
	c_ObjectCall Create;
	c_ObjectCall Draw;
	c_ObjectCall Step;
	c_ObjectCall Alarm[N_ALARMSMAX];
	
	c_Var alarm_count[N_ALARMSMAX];
	c_Var r[N_OBJVARSMAX]; // Object registers
	c_Var id;
	
} c_AnimationObject;

typedef struct {
	c_Var id;
	c_Var h, w;
	c_Data image;
	SDL_Surface* surf;
	SDL_Texture* tex;
} c_AnimationSprite;

typedef enum {
	c_CREATE,
	c_DRAW,
	c_STEP,
	C_ALARM0,
	C_ALARM1,
	C_ALARM2
} c_Event; 

class c_AnimationEngine {
public:
	c_AnimationEngine(c_Var w, c_Var h, c_Var cap);
	
	c_Var CreateObject(void);
	
	c_Var CreateSprite(c_String filename);
	
	void AttachSprite(c_Var id, c_Var sprite);
	void AttachEvent(c_Var id, c_Event ev, c_ObjectCall func);
	void AttachAlarm(c_Var id, c_Var alarm, c_ObjectCall func);
	void SetVariable(c_Var id, c_ObjVar var, c_Var val);
	c_Var GetVariable(c_Var id, c_ObjVar var);
	void SetAlarm(c_Var id, c_Var alarm, c_Var time);
	
	void Animate(void);
	
private:
	c_AnimationObject* objects;
	c_AnimationSprite* sprites;
	c_Var nobjects, nsprites, ticksperframe;
	SDL_Display display_output;
};
#endif

#include "c_anim.h"
#include "l_timer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

c_AnimationEngine::c_AnimationEngine(c_Var w, c_Var h, c_Var cap) {
	this->display_output = SDL_Display(h,w);
	this->objects = (c_AnimationObject*)calloc(N_OBJSMAX, sizeof(c_AnimationObject));
	this->sprites = (c_AnimationSprite*)calloc(N_SPRSMAX, sizeof(c_AnimationSprite));
	this->nsprites = N_SPRSMAX;
	this->nobjects = N_OBJSMAX;
	this->ticksperframe = 1000 / cap;
}

c_Var c_AnimationEngine::CreateObject(void) {
	c_Var id = 0;
	for(unsigned int i = 0; i < this->nobjects; i++) {
		if(this->objects[i].id == 0) {
			id = i + 1;
			this->objects[i].id = id;
			break;
		}
	}
	if(id == 0)
		return 0;
	else
		return id;
}

c_Var c_AnimationEngine::CreateSprite(c_String filename) {
	c_Var id = 0;
	int w, h, n_comp;
	c_Data image_data = stbi_load((char*)filename, &w, &h, &n_comp, 4);
	if(image_data == 0) {
		return 0;
	}
	for(unsigned int i = 0; i < this->nsprites; i++) {
		if(this->sprites[i].id == 0) {
			this->sprites[i].id = i+1;
			id = i+1;
		}
	}
	if(id == 0)
		return 0;
	
	this->sprites[id-1].image = image_data;
	this->sprites[id-1].h = h;
	this->sprites[id-1].w = w;
	
	return id;
}

void c_AnimationEngine::AttachSprite(c_Var id, c_Var sprite) {
	if(id > 0 && id < N_SPRSMAX)
		this->objects[id-1].r[c_ObjSprite] = sprite;
}

void c_AnimationEngine::AttachEvent(c_Var id, c_Event ev, c_ObjectCall func) {
	if(!(id > 0 && id < N_OBJSMAX))
		return;
	
	switch(ev) {
		case c_CREATE:
			this->objects[id-1].Create = func;
			break;
		case c_STEP:
			this->objects[id-1].Step = func;
			break;
		case c_DRAW:
			this->objects[id-1].Draw = func;
			break;
		default:
			break;
	}
}

void c_AnimationEngine::AttachAlarm(c_Var id, c_Var alarm, c_ObjectCall func) {
	if(!(id > 0 && id < N_OBJSMAX))
		return;
	
	if(alarm >= N_ALARMSMAX)
		return;
	
	this->objects[id - 1].Alarm[alarm] = func;
}

void c_AnimationEngine::SetVariable(c_Var id, c_ObjVar var, c_Var val) {
	if(var >= N_OBJVARSMAX)
		return;
	if(!(id > 0 && id < N_OBJSMAX))
		return;
	
	this->objects[id - 1].r[var] = val;
}

c_Var c_AnimationEngine::GetVariable(c_Var id, c_ObjVar var) {
	if(var >= N_OBJVARSMAX)
		return 0;
	if(!(id > 0 && id < N_OBJSMAX))
		return 0;
	
	return this->objects[id - 1].r[var];
}

void c_AnimationEngine::SetAlarm(c_Var id, c_Var alarm, c_Var time) {
	if(alarm >= N_ALARMSMAX)
		return;
	if(!(id > 0 && id < N_OBJSMAX))
		return;
	
	this->objects[id - 1].alarm_count[alarm] = time;
}

void c_AnimationEngine::Animate(void) {
	// First we create SDL surfaces from pixel data
	for(unsigned int i = 0; i < this->nsprites; i++) {
		if(this->sprites[i].id != 0) {
			this->sprites[i].surf = SDL_CreateRGBSurfaceFrom((void*)this->sprites[i].image,
			this->sprites[i].w,
			this->sprites[i].h,
			32,
			this->sprites[i].w*32,
			0x000000FF,
			0x0000FF00,
			0x00FF0000,
			0xFF000000);
			this->sprites[i].tex = display_output.CreateTexFromSurf(this->sprites[i].surf);
		}
	}
	// We are ready for drawing
	this->display_output.Clear(0x0000);
	this->display_output.Flush();
	// Initialize creation routines
	for(unsigned int i = 0; i < this->nobjects; i++) {
		if(this->objects[i].id != 0) {
			if(this->objects[i].Create != NULL) {
				this->objects[i].Create(this);
			}
		}
	}
	// Cap FPS to 60 (or value decided)
	LTimer fps_timer, cap_timer;
	fps_timer.start();
	float avg_fps; int counted_frames;
	
	while(true) {
		cap_timer.start();
		avg_fps = counted_frames /  (fps_timer.getTicks() / 1000.0f);
		if(avg_fps > 200000)
			avg_fps = 0;
		// Call our object functions
		for(unsigned int i = 0; i < this->nobjects; i++) {
			if(this->objects[i].id != 0) {
				// Step first
				if(this->objects[i].Step != NULL) {
					this->objects[i].Step((void*)this);
				}
				// Draw second
				if(this->objects[i].Draw != NULL) {
					this->objects[i].Draw((void*)this);
					if(this->objects[i].r[c_ObjSprite] != 0) {
						display_output.BlitSurface(this->objects[i].r[c_ObjX], this->objects[i].r[c_ObjY], this->sprites[this->objects[i].r[c_ObjSprite]-1].tex);
					}
				}
				// Alarms third
				for(unsigned int j = 0; j < N_ALARMSMAX; i++) {
				if(this->objects[i].alarm_count[j] != 0) {
					this->objects[i].alarm_count[j]--;
					if(this->objects[i].alarm_count[j] == 0)
						this->objects[i].Alarm[j](this);
					}
				}
			}
		}
		// Update SDL
		this->display_output.Update();
		// Cap frame rate	
		++counted_frames;
		int frame_ticks = cap_timer.getTicks();
		if(frame_ticks < this->ticksperframe)
			SDL_Delay(this->ticksperframe - frame_ticks);
		
	}
}

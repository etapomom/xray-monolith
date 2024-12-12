// HUDCrosshair.h:  крестик прицела, отображающий текущую дисперсию
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#define HUD_CURSOR_SECTION "hud_cursor"

#include "ui_defs.h"


class CHUDCrosshair
{
private:
	float cross_length_perc;
	float min_radius_perc;
	float max_radius_perc;
	float radius_lerp_speed;
	float target_alpha;
	float cur_alpha;
	u32 current_color;

	//текущий радиус прицела
	float radius;
	float target_radius;
	float start_time;
	bool hide_crosshair;
	bool prev_state;
	bool is_changing;
#ifdef DEBUG
	float			fb_radius;
#endif
	//ref_geom 		hGeomLine;
	ui_shader hShader;
public:
	u32 cross_color;
	u32 cross_color_enemy;
	u32 cross_color_friend;
	u32 cross_color_neutral;

	CHUDCrosshair();
	~CHUDCrosshair();

	virtual u32 GetCrossColorDefault() { return cross_color; }
	virtual u32 GetCrossColorEnemy() { return cross_color_enemy; }
	virtual u32 GetCrossColorNeutral() { return cross_color_neutral; }
	virtual u32 GetCrossColorFriend() { return cross_color_friend; }
	virtual float GetRadiusLerpSpeed() { return radius_lerp_speed; }
	void SetCurrentColor(u32 c) { current_color = c; }

	void FixAlpha()
	{
		int a = static_cast<int>(round(cur_alpha));
		u32 c = D3DCOLOR_RGBA(color_get_R(current_color), color_get_G(current_color), color_get_B(current_color), a);
		current_color = c;
	}

	void Update();
	void OnRender();
	void SetDispersion(float disp);
#ifdef DEBUG
			void	SetFirstBulletDispertion(float fbdisp);
			void	OnRenderFirstBulletDispertion();
#endif

	void Load();
};

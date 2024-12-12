// HUDCrosshair.cpp:  крестик прицела, отображающий текущую дисперсию
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "HUDCrosshair.h"
//.#include "UIStaticItem.h"
#include "ui_base.h"

#include "Actor.h"
#include "inventory.h"
#include "weapon.h"
#include "level.h"

CHUDCrosshair::CHUDCrosshair()
{
	hShader->create("hud\\crosshair");
	radius = 0;
	cur_alpha = 255.0f;
	hide_crosshair = true;
	prev_state = false;
}


CHUDCrosshair::~CHUDCrosshair()
{
}

void CHUDCrosshair::Load()
{
	//все размеры в процентах от длины экрана
	//длина крестика 
	cross_length_perc = pSettings->r_float(HUD_CURSOR_SECTION, "cross_length");
	min_radius_perc = pSettings->r_float(HUD_CURSOR_SECTION, "min_radius");
	max_radius_perc = pSettings->r_float(HUD_CURSOR_SECTION, "max_radius");
	cross_color = pSettings->r_fcolor(HUD_CURSOR_SECTION, "cross_color").get();

	// momopate
	cross_color_enemy = D3DCOLOR_RGBA(255, 0, 0, 255);
	cross_color_friend = D3DCOLOR_RGBA(0, 255, 0, 255);
	cross_color_neutral = D3DCOLOR_RGBA(255, 255, 0, 255);
	radius_lerp_speed = 1.0f;
	target_alpha = 255.0f;
	if (pSettings->line_exist(HUD_CURSOR_SECTION, "cross_color_enemy"))
		cross_color_enemy = pSettings->r_fcolor(HUD_CURSOR_SECTION, "cross_color_enemy").get();
	if (pSettings->line_exist(HUD_CURSOR_SECTION, "cross_color_friend"))
		cross_color_friend = pSettings->r_fcolor(HUD_CURSOR_SECTION, "cross_color_friend").get();
	if (pSettings->line_exist(HUD_CURSOR_SECTION, "cross_color_neutral"))
		cross_color_neutral = pSettings->r_fcolor(HUD_CURSOR_SECTION, "cross_color_neutral").get();
	if (pSettings->line_exist(HUD_CURSOR_SECTION, "radius_lerp_speed"))
		radius_lerp_speed = pSettings->r_float(HUD_CURSOR_SECTION, "radius_lerp_speed");
	if (pSettings->line_exist(HUD_CURSOR_SECTION, "target_alpha"))
		target_alpha = pSettings->r_float(HUD_CURSOR_SECTION, "alpha");
	current_color = cross_color;
}

//выставляет radius от min_radius до max_radius
void CHUDCrosshair::SetDispersion(float disp)
{
	Fvector4 r;
	Fvector R = {VIEWPORT_NEAR * _sin(disp), 0.f, VIEWPORT_NEAR};
	Device.mProject.transform(r, R);

	Fvector2 scr_size;
	scr_size.set(float(::Render->getTarget()->get_width()), float(::Render->getTarget()->get_height()));
	float radius_pixels = _abs(r.x) * scr_size.x / 2.0f;
	target_radius = radius_pixels;
}

#ifdef DEBUG
void CHUDCrosshair::SetFirstBulletDispertion(float fbdisp)
{
	Fvector4 r;
	Fvector R			= { VIEWPORT_NEAR*_sin(fbdisp), 0.f, VIEWPORT_NEAR };
	Device.mProject.transform	(r,R);

	Fvector2		scr_size;
	scr_size.set	(float(::Render->getTarget()->get_width()), float(::Render->getTarget()->get_height()));
	fb_radius		= _abs(r.x)*scr_size.x/2.0f;
}

BOOL	g_bDrawFirstBulletCrosshair = FALSE;

void CHUDCrosshair::OnRenderFirstBulletDispertion()
{
	VERIFY			(g_bRendering);
	Fvector2		center;
	Fvector2		scr_size;
	scr_size.set	(float(::Render->getTarget()->get_width()), float(::Render->getTarget()->get_height()));
	center.set		(scr_size.x/2.0f, scr_size.y/2.0f);

	UIRender->StartPrimitive		(10, IUIRender::ptLineList, UI().m_currentPointType);

	u32	fb_cross_color				= color_rgba(255, 0, 0, 255); //red
	

	float cross_length				= /*cross_length_perc*/0.008f*scr_size.x;
	float min_radius				= min_radius_perc*scr_size.x;
	float max_radius				= max_radius_perc*scr_size.x;

	clamp							(target_radius , min_radius, max_radius);

	float x_min						= min_radius + fb_radius;
	float x_max						= x_min + cross_length;

	float y_min						= x_min;
	float y_max						= x_max;

	// 0
	UIRender->PushPoint(center.x,			center.y + y_min,	0, fb_cross_color, 0,0);
	UIRender->PushPoint(center.x,			center.y + y_max,	0, fb_cross_color, 0,0);
	// 1
	UIRender->PushPoint(center.x,			center.y - y_min,	0, fb_cross_color, 0,0);
	UIRender->PushPoint(center.x,			center.y - y_max,	0, fb_cross_color, 0,0);
	// 2
	UIRender->PushPoint(center.x + x_min,	center.y,			0, fb_cross_color, 0,0);
	UIRender->PushPoint(center.x + x_max,	center.y,			0, fb_cross_color, 0,0);
	// 3
	UIRender->PushPoint(center.x - x_min,	center.y,			0, fb_cross_color, 0,0);
	UIRender->PushPoint(center.x - x_max,	center.y,			0, fb_cross_color, 0,0);
	
	// point
	UIRender->PushPoint(center.x-0.5f,		center.y,			0, fb_cross_color, 0,0);
	UIRender->PushPoint(center.x+0.5f,		center.y,			0, fb_cross_color, 0,0);


	// render	
	UIRender->SetShader						(*hShader);
	UIRender->FlushPrimitive				();
}
#endif

extern ENGINE_API BOOL g_bRendering;

void CHUDCrosshair::OnRender()
{
	VERIFY(g_bRendering);
	Fvector2 center;
	Fvector2 scr_size;
	scr_size.set(float(::Render->getTarget()->get_width()), float(::Render->getTarget()->get_height()));
	center.set(scr_size.x / 2.0f, scr_size.y / 2.0f);

	UIRender->StartPrimitive(10, IUIRender::ptLineList, UI().m_currentPointType);


	float cross_length = cross_length_perc * scr_size.x;
	float min_radius = min_radius_perc * scr_size.x;
	float max_radius = max_radius_perc * scr_size.x;

	clamp(target_radius, min_radius, max_radius);

	float x_min = min_radius + radius;
	float x_max = x_min + cross_length;

	float y_min = x_min;
	float y_max = x_max;

	// 0
	UIRender->PushPoint(center.x, center.y + y_min, 0, current_color, 0, 0);
	UIRender->PushPoint(center.x, center.y + y_max, 0, current_color, 0, 0);
	// 1
	UIRender->PushPoint(center.x, center.y - y_min, 0, current_color, 0, 0);
	UIRender->PushPoint(center.x, center.y - y_max, 0, current_color, 0, 0);
	// 2
	UIRender->PushPoint(center.x + x_min, center.y, 0, current_color, 0, 0);
	UIRender->PushPoint(center.x + x_max, center.y, 0, current_color, 0, 0);
	// 3
	UIRender->PushPoint(center.x - x_min, center.y, 0, current_color, 0, 0);
	UIRender->PushPoint(center.x - x_max, center.y, 0, current_color, 0, 0);

	// point
	UIRender->PushPoint(center.x - 0.5f, center.y, 0, current_color, 0, 0);
	UIRender->PushPoint(center.x + 0.5f, center.y, 0, current_color, 0, 0);


	// render	
	UIRender->SetShader(*hShader);
	UIRender->FlushPrimitive();


	if (!fsimilar(target_radius, radius))
	{
		float sp = 1.0f * scr_size.x;
        float radius_change = sp * Device.fTimeDelta;
        clamp(radius_change, 0.0f, sp * 0.0069444f); // clamp to 30 fps
        clamp(radius_change, 0.0f, _abs(target_radius - radius));

        if (target_radius < radius)
            radius -= radius_change;
        else
            radius += radius_change;
	};
#ifdef DEBUG
	if (g_bDrawFirstBulletCrosshair)
		OnRenderFirstBulletDispertion();
#endif
}

void CHUDCrosshair::Update()
{
	VERIFY(g_bRendering);

	bool dummy = false;
	CActor* a = smart_cast<CActor*>(Level().CurrentEntity());
	if (a)
	{
		CWeapon* w = smart_cast<CWeapon*>(a->inventory().ActiveItem());
		if (w)
		{
			u32 s = w->GetState();
			hide_crosshair = 	(w->IsHiding() || w->IsHidden() || a->is_safemode() || a->IsZoomAimingMode()) ||
								(s == CHUDState::eHiding || s == CHUDState::eHidden || s == CWeapon::eReload) ||
								a->GetFireDispertion() > 0.349066f;	// 20deg
			dummy = w->GetInertionAimFactor() < 1.0f;
		}
		else
			hide_crosshair = true;
	}
	else
		return;
	
	if (hide_crosshair && !dummy)
		SetDispersion(0.349066f);	// 10deg
	
	if (hide_crosshair != prev_state || is_changing)
	{
		if (!is_changing)
		{
			start_time = Device.dwTimeGlobal - 1.0f;
			is_changing = true;
		}
		else
		{
			float cur_time = Device.dwTimeGlobal;
			float end_time = start_time + 151.0f;

			if (cur_time >= end_time)
				cur_alpha = (hide_crosshair) ? 0.0f : target_alpha;
			
			if (hide_crosshair)
				cur_alpha = _lerp(target_alpha, 0.0f, (cur_time - start_time) / (end_time - start_time));
			else
				cur_alpha = _lerp(0.0f, target_alpha, (cur_time - start_time) / (end_time - start_time));
			
			if (cur_alpha < 0.0f)
				cur_alpha = 0.0f;
			else if (cur_alpha > 255.0f)
				cur_alpha = 255.0f;
			else if (cur_alpha > target_alpha)
				cur_alpha = target_alpha;
			
			if (((cur_alpha == 0.0f && hide_crosshair) || (cur_alpha == 255.0f && !hide_crosshair) || (cur_alpha == target_alpha)) && (cur_time != start_time))
				is_changing = false;
		}
	}

	prev_state = hide_crosshair;
}
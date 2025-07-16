#include <raylib.h>
#include <format>

#include "kwecs.h"

struct position
{
	float x;
	float y;
};

struct velocity
{
	float x;
	float y;
};

struct ball {};
struct player1 {};
struct player2 {};

struct score_counter
{
	uint32_t player1_score = 0;
	uint32_t player2_score = 0;
};

void handle_player1_input(player1&, velocity& vel)
{
	if (IsKeyDown(KEY_W)) vel.y = -1;
	else if (IsKeyDown(KEY_S)) vel.y = 1;
	else vel.y = 0;
}


void handle_player2_input(player2&, velocity& vel)
{
	if (IsKeyDown(KEY_UP)) vel.y = -1;
	else if (IsKeyDown(KEY_DOWN)) vel.y = 1;
	else vel.y = 0;
}



void handle_players_movement(float delta_time, float players_speed, position& pos, const velocity& vel, player1* pl1, player2* pl2)
{
	if (pl1 || pl2)
	{
		pos.x += vel.x * players_speed * delta_time;
		pos.y += vel.y * players_speed * delta_time;

		if (pos.y >= 800 - 40)
		{
			pos.y = 800 - 40;
		}

		if (pos.y <= 0)
		{
			pos.y = 0;
		}
	}
}

void handle_ball_movenet(kawa::ecs::registry& reg, float delta_time, float ball_speed, ball&, position& pos, velocity& vel)
{
	pos.x += vel.x * ball_speed * delta_time;
	pos.y += vel.y * ball_speed * delta_time;

	if (pos.x >= 800)
	{
		vel.x *= -1;
		pos = { 400, 400 };
		reg.query([](score_counter& sc) {sc.player1_score++; });
	}

	if (pos.x <= 0)
	{
		vel.x *= -1;
		pos = { 400, 400 };
		reg.query([](score_counter& sc) {sc.player2_score++; });
	}

	if (pos.y >= 800 || pos.y <= 0)
	{
		vel.y *= -1;
	}

	reg.query
	(
		[&](position& p_pos, player1* pl1, player2* pl2)
		{
			if (pl1)
			{
				if (pos.x <= p_pos.x + 20 && pos.x >= p_pos.x)
				{
					if ((pos.y >= p_pos.y - 10) && (pos.y <= p_pos.y + 50))
					{
						vel.x = 0.5;
					}
				}
			}


			if (pl2)
			{
				if (pos.x >= p_pos.x && pos.x <= p_pos.x + 20)
				{
					if ((pos.y >= p_pos.y - 10) && (pos.y <= p_pos.y + 50))
					{
						vel.x = -0.5;
					}
				}
			}
		}
	);

}

void draw_players(position& pos, player1* pl1, player2* pl2)
{
	if (pl1)
	{
		DrawRectangle(pos.x - 20, pos.y, 10, 40, RED);
	}

	if (pl2)
	{
		DrawRectangle(pos.x - 20, pos.y, 10, 40, BLUE);
	}
}

void draw_ball(ball&, position& pos)
{
	DrawCircle(pos.x - 20, pos.y, 10, WHITE);
}

void draw_score(score_counter& sc, position& pos)
{
	DrawText(std::format("{} : {}", sc.player1_score, sc.player2_score).c_str(), pos.x - 42, pos.y, 42, BLACK);
}

int main()
{
	const float ball_speed = 300;
	const float players_speed = 300;

	using namespace kawa::ecs;

	registry reg({});

	reg.entity_with(ball{}, position{ 400,400 }, velocity{ -0.5, 0.5 });
	reg.entity_with(player1{}, position{ 100,400 }, velocity{ 0,0 });
	reg.entity_with(player2{}, position{ 700,400 }, velocity{ 0,0 });
	reg.entity_with(score_counter{}, position{ 400,100 });

	InitWindow(800, 800, "kawa::ecs ping-pong");

	SetTargetFPS(144);

	float delta_time = 0;
	while (!WindowShouldClose())
	{
		delta_time = GetFrameTime();
		BeginDrawing();

		reg.query(handle_player1_input);
		reg.query(handle_player2_input);
		reg.query(handle_players_movement, delta_time, players_speed);
		reg.query(handle_ball_movenet, reg, delta_time, ball_speed);
		reg.query(draw_players);
		reg.query(draw_ball);
		reg.query(draw_score);

		ClearBackground(LIGHTGRAY);
		EndDrawing();
	}
	CloseWindow();
}

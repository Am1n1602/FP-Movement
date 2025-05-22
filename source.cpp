#include <raylib.h>
#include <raymath.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

int main()
{
	const int SCREENWIDTH = 1280;
	const int SCREENHEIGHT = 720;

	InitWindow(SCREENWIDTH, SCREENHEIGHT, "FP-Movement");

	ToggleFullscreen();
	DisableCursor();
	SetTargetFPS(60);

	Vector2 PlayerPos = { 1,1 };

	float PlayerLook = 0.0f;
	float POV = 3.14 / 4.0f;
	float Depth = 16.0f;

	float speed = 0.1f;

	int MapHeight = 16;
	int MapWidth = 16;

	int MaxColumn = 5;
	int CurrentColumn = 0;
	int MapColumn = -1;

		// Minimap

		std::string map;
		for (int i = 0;i < MapWidth;i++) {
			if (CurrentColumn <= MaxColumn && i>4 && i<12) {
				MapColumn = GetRandomValue(1, 14);
				CurrentColumn++;
			}
			else {
				MapColumn = -1;
			}
			if (i == 0 || i == MapWidth - 1) {
				for (int j = 0;j < MapHeight;j++) {
					map += '#';
				}
			}
			else {
				for (int j = 0;j < MapHeight;j++) {
					if (j == 0 || j == MapHeight - 1) {
						map += '#';
					}
					else if (j == MapColumn) {
						map += '#';
					}
					else
					{
						map += '.';
					}
				}
			}
		}

	while (!WindowShouldClose())
	{

		// Handle Rotations

		if (IsKeyDown(KEY_A)) {
			PlayerLook -= speed*(0.75);
		}
		if (IsKeyDown(KEY_D)) {
			PlayerLook += speed*(0.75);
		}

		// Handle Movement

		if (IsKeyDown(KEY_W)) {
			PlayerPos.x += sinf(PlayerLook) * speed;
			PlayerPos.y += cosf(PlayerLook) * speed;

			if (map[(int)PlayerPos.y * MapWidth + (int)PlayerPos.x] == '#') {
				PlayerPos.x -= sinf(PlayerLook) * speed;
				PlayerPos.y -= cosf(PlayerLook) * speed;
			}
		}

		if (IsKeyDown(KEY_S)) {
			PlayerPos.x -= sinf(PlayerLook) * speed;
			PlayerPos.y -= cosf(PlayerLook) * speed;

			if (map[(int)PlayerPos.y * MapWidth + (int)PlayerPos.x] == '#') {
				PlayerPos.x += sinf(PlayerLook) * speed;
				PlayerPos.y += cosf(PlayerLook) * speed;
			}

		}

		if (IsKeyDown(KEY_LEFT_SHIFT)) {
			speed = 0.3f;
		}

		if (IsKeyUp(KEY_LEFT_SHIFT)) {
			speed = 0.1f;
		}

		BeginDrawing();
		ClearBackground(RAYWHITE);


		for (int x = 0;x < SCREENWIDTH;x++) {

			float RayAngle = (PlayerLook - POV / 2.0f) + ((float)x / (float)SCREENWIDTH) * POV;

			float DistanceToWall = 1.0f;

			bool FhitWallCheck = false;
			bool FhitBoundaryCheck = false;

			Vector2 PlayerView = { sinf(RayAngle),cosf(RayAngle) };

			while (!FhitWallCheck && DistanceToWall < Depth) {
				DistanceToWall += 0.1f;
				int nTestX = (int)(PlayerPos.x + PlayerView.x * DistanceToWall);
				int nTestY = (int)(PlayerPos.y+ PlayerView.y * DistanceToWall);

				// Test if ray is out of bound
				if (nTestX < 0 || nTestX >= MapWidth || nTestY < 0 || nTestY >= MapHeight) {
					FhitWallCheck = true;
					DistanceToWall = Depth;
				}
				else {
					if (map[nTestY * MapWidth + nTestX] == '#') {
						FhitWallCheck = true;

						std::vector<std::pair<float, float>> PBoundary; // distance and dot product
						for (int tx = 0; tx < 2; tx++) {
							for (int ty = 0; ty < 2; ty++) {
								float vy = float(nTestY + ty - PlayerPos.y);
								float vx = float(nTestX + tx - PlayerPos.x);
								float distance = sqrt(vx * vx + vy * vy);
								float dot = (PlayerView.y * vx / distance) + (PlayerView.y * vy / distance);
								PBoundary.push_back(std::make_pair(distance, dot));
							}
						}
						// sort pairs from closest to farthest
						std::sort(PBoundary.begin(), PBoundary.end(), [](const std::pair<float, float>& left, const std::pair<float, float>& right) {
							return left.first < right.first;
							});
						float Fbound = 0.01f;
						if (acos(PBoundary.at(0).second) < Fbound) {
							FhitBoundaryCheck = true;
						}
						if (acos(PBoundary.at(1).second) < Fbound) {
							FhitBoundaryCheck = true;
						}
					}
				}
			}

			int CeilingHeight = (float)(SCREENHEIGHT / 2.5f) - (SCREENHEIGHT / ((float)DistanceToWall));
		int FloorHeight = SCREENHEIGHT - CeilingHeight;
			Color wallColor;

			if (DistanceToWall <= Depth / 4.0f) {
				wallColor = YELLOW; // very close so dark
			}
			else if (DistanceToWall < Depth / 3.5f) {
				wallColor = ColorAlpha(YELLOW, 0.9f);
			}
			else if (DistanceToWall < Depth / 3.0f) {
				wallColor = ColorAlpha(YELLOW, 0.8f);
			}
			else if (DistanceToWall < Depth/2.5f) {
				wallColor = ColorAlpha(YELLOW, 0.7f); 
			}
			else if (DistanceToWall < Depth / 2.0f) {
				wallColor = ColorAlpha(YELLOW, 0.6f);
			}
			else if (DistanceToWall < Depth / 1.5f) {
				wallColor = ColorAlpha(YELLOW, 0.5f);
			}
			else if (DistanceToWall < Depth) {
				wallColor = ColorAlpha(YELLOW, 0.4f); // far so light
			}
			else {
				wallColor = ColorAlpha(YELLOW,0.2f);
			}

			if (FhitBoundaryCheck) {
				wallColor = YELLOW;
			}


			for (int y = 0;y < SCREENHEIGHT;y++) {
				if (y < CeilingHeight) {
					DrawPixel(x,y,BLUE);
				}
				else if (y > CeilingHeight && y <= FloorHeight) {
					DrawPixel(x,y,wallColor);
				}
				else {
					Color floorColor;
					float b = 1.0f - (((float)y - SCREENHEIGHT / 2.0f) / ((float)SCREENHEIGHT / 2.0f));
					if (b < 0.25) {
						floorColor = ColorAlpha(RED,0.9f);
					}
					else if (b < 0.5) {
						floorColor = ColorAlpha(RED, 0.7f);
					}
					else if (b < 0.75) {
						floorColor = ColorAlpha(RED, 0.5f);;
					}
					else if (b < 0.95) {
						floorColor = ColorAlpha(RED, 0.3f);
					}
					else {
						floorColor = RED; 
					}
					DrawPixel(x, y, RED);
				}
			}
		}


		// MiniMap

		for (int i = 0;i < MapWidth;i++) {
			for (int j = 0;j < MapHeight;j++) {
				if (map[j * MapWidth + i] == '#') {
					DrawRectangle(i * 10 + 10, j * 10 + 50, 10, 10, DARKGRAY);
				}
				else {
					DrawRectangle(i * 10 + 10, j * 10 + 50, 10, 10, LIGHTGRAY);
				}
			}
		}
		DrawCircle((int)PlayerPos.x * 10 + 15, (int)PlayerPos.y * 10 + 55, 3, RED);
		EndDrawing();

	}		

	void CloseWindow();
	
}
#ifndef SNAKEGAME_HPP
#define SNAKEGAME_HPP

#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include "GFX.hpp"

class SnakeGame : public GameBehaviour
{
private:
	enum Direction
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		NONE
	};

	struct Segment
	{
		int x, y;
		Segment(int x, int y) : x(x), y(y) {}
	};

	std::vector<Segment> snake;
	Direction currentDir;
	Direction nextDir;
	int foodX, foodY;
	int score;
	bool gameOver;
	float moveTimer;
	const float moveInterval = 0.175f;
	int gridSize;
	Font *gameFont;

	bool CheckSelfCollision(int x, int y)
	{
		for (const auto &segment : snake)
			if (segment.x == x && segment.y == y)
				return true;
		return false;
	}

	void SpawnFood()
	{
		do
		{
			foodX = rand() % gridSize;
			foodY = rand() % gridSize;
		} while (CheckSelfCollision(foodX, foodY));
	}

protected:
	void OnInitialize() override
	{
		Camera::GetMain()->SetClearColor(Color::Black());
		gameFont = Resources::FindFont(Constants::GetString(ConstantString::FontDefault));
		gridSize = 20;
		currentDir = RIGHT;
		nextDir = RIGHT;
		score = 0;
		gameOver = false;
		moveTimer = 0;
		srand(static_cast<unsigned>(time(nullptr)));
		Reset();
	}

	void SetFont(Font *font) { gameFont = font; }

	void Reset()
	{
		snake.clear();
		snake.emplace_back(gridSize / 2, gridSize / 2);
		currentDir = RIGHT;
		nextDir = RIGHT;
		score = 0;
		gameOver = false;		
		moveTimer = 0;
		SpawnFood();
	}

	void HandleInput()
	{
		if (gameOver)
		{
			if (Input::GetKeyDown(KeyCode::R))
				Reset();
			return;
		}

		if (Input::GetKey(KeyCode::Up) && currentDir != DOWN)
			nextDir = UP;
		else if (Input::GetKey(KeyCode::Down) && currentDir != UP)
			nextDir = DOWN;
		else if (Input::GetKey(KeyCode::Left) && currentDir != RIGHT)
			nextDir = LEFT;
		else if (Input::GetKey(KeyCode::Right) && currentDir != LEFT)
			nextDir = RIGHT;
	}

	void OnUpdate() override
	{
		HandleInput();

		if (gameOver)
			return;

		moveTimer += Time::GetDeltaTime();

		if (moveTimer >= moveInterval)
		{
			moveTimer = 0;
			currentDir = nextDir;

			Segment newHead = snake.front();
			switch (currentDir)
			{
			case UP:
				newHead.y--;
				break;
			case DOWN:
				newHead.y++;
				break;
			case LEFT:
				newHead.x--;
				break;
			case RIGHT:
				newHead.x++;
				break;
			case NONE:
				return;
			}

			// Check collisions
			if (newHead.x < 0 || newHead.x >= gridSize ||
				newHead.y < 0 || newHead.y >= gridSize ||
				CheckSelfCollision(newHead.x, newHead.y))
			{
				gameOver = true;
				return;
			}

			snake.insert(snake.begin(), newHead);

			if (newHead.x == foodX && newHead.y == foodY)
			{
				score++;
				SpawnFood();
			}
			else
			{
				snake.pop_back();
			}
		}
	}

	void OnLateUpdate() override
	{
		Render();
	}

	void Render()
	{
		Rectangle viewport = Graphics::GetViewport();
		const float cellSize = glm::min(viewport.width, viewport.height) / gridSize;
		const float gridOffsetX = (viewport.width - cellSize * gridSize) / 2;
		const float gridOffsetY = (viewport.height - cellSize * gridSize) / 2;

		// Draw border
		Graphics2D::AddBorder(
			Vector2(gridOffsetX, gridOffsetY),
			Vector2(cellSize * gridSize, cellSize * gridSize),
			2.0f,
			BorderOptions_All,
			Color::White());

		// Draw snake
		for (const auto &segment : snake)
		{
			const float x = gridOffsetX + segment.x * cellSize;
			const float y = gridOffsetY + segment.y * cellSize;

			Graphics2D::AddRectangle(
				Vector2(x, y),
				Vector2(cellSize, cellSize),
				0.0f,
				Color::LightGray());

			// Graphics2D::AddRectangleRounded(
			// 	Vector2(x, y),
			// 	Vector2(cellSize, cellSize),
			// 	0.0f,
			// 	6.0f,
			// 	(segment.x == snake.front().x && segment.y == snake.front().y) ? Color::Green() : Color::Green());
		}

		// Draw food
		const float foodXPos = gridOffsetX + foodX * cellSize + cellSize / 2;
		const float foodYPos = gridOffsetY + foodY * cellSize + cellSize / 2;
		Graphics2D::AddCircle(
			Vector2(foodXPos, foodYPos),
			cellSize / 2 * 0.8f,
			16,
			0.0f,
			Color::Red());

		// Draw score
		if (gameFont)
		{
			std::string scoreText = "Score: " + std::to_string(score);
			Graphics2D::AddText(
				Vector2(gridOffsetX, gridOffsetY + 10),
				gameFont,
				scoreText,
				24,
				Color::White(),
				false);
		}

		// Draw game over screen
		if (gameOver && gameFont)
		{
			const std::string gameOverText = "Game Over! Press R to restart";
			//float fontSize  = 36;

			float fontSize = cellSize * 0.95f;

			Vector2 position = GetPositionFromRectangle(viewport);
			Vector2 textSize = CalculateTextSize(gameOverText, fontSize);
			Vector2 textPosition = position + CalculateCenteredPosition(viewport, textSize);

			Graphics2D::AddText(
				textPosition,
				gameFont,
				gameOverText,
				fontSize,
				Color::Red(),
				false);
		}
	}

	Vector2 CalculateTextSize(const std::string &text, float fontSize)
	{
		Vector2 bounds;
		gameFont->CalculateBounds(text.c_str(), text.size(), fontSize, bounds.x, bounds.y);
		return bounds;
	}

	Vector2 CalculateCenteredPosition(const Rectangle &rect, Vector2 size)
	{
		return Vector2((rect.width - size.x) * 0.5f, (rect.height - size.y) * 0.5f);
	}

	Vector2 GetPositionFromRectangle(const Rectangle &rect)
	{
		return Vector2(rect.x, rect.y);
	}

	Vector2 GetSizeFromRectangle(const Rectangle &rect)
	{
		return Vector2(rect.width, rect.height);
	}
};

#endif
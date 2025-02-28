#include "GUITextBuffer.hpp"

namespace GFX
{
	GUITextBuffer::GUITextBuffer()
	{
		options.textType = TextType::Normal;
		blinkTimer = 0.0f;
		blinkInterval = 0.25f;
		lastKeyStroke = 0.0f;
		multiLine = true;
		readOnly = false;
	}

	void GUITextBuffer::SetCursorIndex(int index) 
	{
		if(index > text.size()) 
			index = text.size();

		cursorIndex = index;
	}

	void GUITextBuffer::GetCursorPosition(int &row, int &column)
	{
		row = 0;
		column = 0;
		int index = 0;

		// Iterate through the text to find the current row and column
		while (index < cursorIndex && index < text.size()) 
		{
			if (text[index] == '\n') 
			{
				++row; // Move to the next row
				column = 0; // Reset column count
			} 
			else 
			{
				++column; // Move to the next character in the current line
			}
			++index; // Move to the next character
		}
	}

	void GUITextBuffer::SetCursorPosition(int row, int column)
	{
		int currentRow = 0;
		int index = 0;

		// Iterate through the text to find the specified row
		while (currentRow < row && index < text.length())
		{
			if (text[index] == '\n')
			{
				currentRow++; // Move to the next row
			}
			index++; // Move to the next character
		}

		// If we have found the desired row, set the cursor
		if (currentRow == row)
		{
			// If column is out of bounds, set to the end of the line
			size_t lineStart = (currentRow > 0) ? text.rfind('\n', index - 1) + 1 : 0;
			size_t lineEnd = text.find('\n', lineStart);

			if (lineEnd == std::string::npos)
			{
				lineEnd = text.length(); // Last line goes to end of text
			}

			// Limit the cursor index to the length of the current line
			cursorIndex = lineStart + std::min(column, static_cast<int>(lineEnd - lineStart));
		}
		else
		{
			// If the row exceeds the total number of lines, set to end of text
			cursorIndex = text.length();
		}
	}

	void GUITextBuffer::ResetLastKeyStroke(float elapsedTime) 
	{
		lastKeyStroke = elapsedTime;
		blinkTimer = 0;
	}

	Vector2 GUITextBuffer::CalculateTextPosition(const Rectangle &rect, Font *font, float fontSize, const Vector2 &textOffset) 
	{
		Vector2 position(rect.x, rect.y);
		Vector2 size(rect.width, rect.height);
		Vector2 cursorPosition(position.x + textOffset.x, position.y + textOffset.y);
		Vector2 textPosition(position.x + textOffset.x, position.y + textOffset.y);
		font->CalculateCharacterPosition(text.c_str(), text.size(), cursorIndex, fontSize, cursorPosition.x, cursorPosition.y);
		float rectangleBottom = position.y + size.y;
		float rectangleRight = position.x + size.x;
		float cursorWidth = 2.0f;
		float cursorHeight = font->GetMaxHeight() * font->GetPixelScale(fontSize);

		auto absf = [] (float v) -> float {
			if(v < 0.0f)
				return v * -1.0f;
			return v;
		};

		float cursorX = cursorPosition.x + cursorWidth;
		float cursorY = cursorPosition.y + cursorHeight;

		if(cursorY > rectangleBottom) 
		{
			float distanceBelow = absf(cursorY - rectangleBottom); // Distance from bottom of rectangle to cursor
			int rowsBelow = (int)(distanceBelow / cursorHeight) + 1; // Number of rows below
			textPosition.y -= rowsBelow * cursorHeight;
		}

		if(cursorX > rectangleRight) 
		{
			float distanceRight = absf(cursorX - rectangleRight); // Distance from right edge to cursor
			int columnsRight = (int)(distanceRight / cursorWidth) + 3; // Number of columns outside
			textPosition.x -= (columnsRight * cursorWidth);
		}

		return textPosition;
	}

	Vector2 GUITextBuffer::CalculateCursorPosition(const Rectangle &rect, Font *font, float fontSize, const Vector2 &textOffset)
	{
		Vector2 position(rect.x, rect.y);
		Vector2 size(rect.width, rect.height);
		Vector2 cursorPosition(position.x + textOffset.x, position.y + textOffset.y);
		font->CalculateCharacterPosition(text.c_str(), text.size(), cursorIndex, fontSize, cursorPosition.x, cursorPosition.y);
		float rectangleBottom = position.y + size.y;
		float rectangleRight = position.x + size.x;
		float cursorWidth = 2.0f;
		float cursorHeight = font->GetMaxHeight() * font->GetPixelScale(fontSize);

		auto absf = [] (float v) -> float {
			if(v < 0.0f)
				return v * -1.0f;
			return v;
		};

		float cursorX = cursorPosition.x + cursorWidth;
		float cursorY = cursorPosition.y + cursorHeight;

		if(cursorY > rectangleBottom) 
		{
			float distanceBelow = absf(cursorY - rectangleBottom); // Distance from bottom of rectangle to cursor
			int rowsBelow = (int)(distanceBelow / cursorHeight) + 1; // Number of rows below
			cursorPosition.y -= rowsBelow * cursorHeight;
		}

		if(cursorX > rectangleRight) 
		{
			float distanceRight = absf(cursorX - rectangleRight); // Distance from right edge to cursor
			int columnsRight = (int)(distanceRight / cursorWidth) + 3; // Number of columns outside
			cursorPosition.x -= (columnsRight * cursorWidth);
		}

		return cursorPosition;
	}

	int GUITextBuffer::GetIndexOfNextNewLineChar()
	{
		// Start searching from the cursorIndex to the end of the string
		for (int i = cursorIndex; i < text.length(); ++i) 
		{
			if (text[i] == '\n') 
			{
				return i; // Return the index of the newline character
			}
		}
		return text.length(); // No next newline found
	}

	int GUITextBuffer::GetIndexOfPreviousNewLineChar() 
	{
		// Start searching from the cursorIndex - 1 to avoid GUITextBuffer::counting the current position
		for (int i = cursorIndex; i > 0; --i) 
		{
			if (text[i - 1] == '\n') 
			{
				return i; // Return the index of the newline character
			}
		}
		return 0; // No previous newline found
	}

	void GUITextBuffer::HandleKeyDown(KeyCode keycode)
	{
		switch (keycode)
		{
			case KeyCode::Backspace:
			{
				if (cursorIndex > 0 && cursorIndex <= text.length() && text.length() > 0 && !readOnly) 
				{
					text.erase(cursorIndex - 1, 1); // Remove 1 character at position (cursorIndex - 1)
					SetCursorIndex(cursorIndex - 1);
					ResetLastKeyStroke(Time::GetTime());
				}
				break;
			}
			case KeyCode::Delete:
			{
				if (cursorIndex < text.length() && !readOnly) 
				{
					text.erase(cursorIndex, 1); // Remove 1 character at cursorIndex
					ResetLastKeyStroke(Time::GetTime());
				}
				break;
			}
			case KeyCode::Left:
			{
				if (cursorIndex >= 1) 
				{
					SetCursorIndex(cursorIndex - 1);
					ResetLastKeyStroke(Time::GetTime());
				}
				break;
			}
			case KeyCode::Right:
			{
				if (cursorIndex < text.length()) 
				{
					SetCursorIndex(cursorIndex + 1);
					ResetLastKeyStroke(Time::GetTime());
				}
				break;
			}
			case KeyCode::Up:
			{
				int row, column;
				GetCursorPosition(row, column);
				if (row > 0) 
				{
					SetCursorPosition(row - 1, column);
					ResetLastKeyStroke(Time::GetTime());
				} 
				else 
				{
					SetCursorPosition(0, 0);
					ResetLastKeyStroke(Time::GetTime());
				}
				break;
			}
			case KeyCode::Down:
			{
				int row, column;
				GetCursorPosition(row, column);
				SetCursorPosition(row + 1, column);
				ResetLastKeyStroke(Time::GetTime());
				break;
			}
			case KeyCode::Home:
			{
				int newCursorIndex = GetIndexOfPreviousNewLineChar();
				SetCursorIndex(newCursorIndex);
				ResetLastKeyStroke(Time::GetTime());
				break;
			}
			case KeyCode::End:
			{
				int newCursorIndex = GetIndexOfNextNewLineChar();
				SetCursorIndex(newCursorIndex);
				ResetLastKeyStroke(Time::GetTime());
				break;
			}
			case KeyCode::Enter:
			{
				if (multiLine && !readOnly) 
				{
					text.insert(cursorIndex, "\n"); // Insert a new line at cursorIndex
					SetCursorIndex(cursorIndex + 1);
					ResetLastKeyStroke(Time::GetTime());
				}
				break;
			}
		}
	}
}
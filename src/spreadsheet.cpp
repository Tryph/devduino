/** MIT License
*
* Copyright(c) 2017 DevDuino
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "spreadsheet.h"

namespace devduino {
	//------------------------------------------------------------------------//
	//---------------------------- Public methods ----------------------------//
	//------------------------------------------------------------------------//
	Spreadsheet::Spreadsheet(const Console& console, uint8_t nbRows, uint8_t nbColumns, bool drawBorders) :
		console(console),
		//Because "begin" has not yet been called, we must not set autoFlush to "true" or setGrid() would like to flush without Wire being intialised.
		autoFlush(false),
		drawBorders(drawBorders)
	{
		setGrid(nbRows, nbColumns);
	}

	Spreadsheet& Spreadsheet::write(uint8_t cellId, const char* value, size_t bufferSize) {
		uint8_t row = cellId / nbColumns;
		uint8_t column = cellId - (row * nbColumns);

		const Display& display = console.getDisplay();
		if (row < nbRows && column < nbColumns) {
			float cellWidth = ((float) display.getWidth()) / nbColumns;
			float cellHeight = ((float) display.getHeight()) / nbRows;

			uint8_t cellX = column * cellWidth;
			float cellY = (display.getHeight() - ((row + 1) * cellHeight));

			display.clearArea(cellX + 1, cellY + 1, cellWidth - 2, cellHeight - 2);

			//Center text y in cell.
			uint8_t textY = cellY + (cellHeight / 2) - ((float) console.getFontSize()) * (((float) console.getFont()->getSize()) / 2);
			// Add +2 to add a space between border and text.
			console.setTextPosition(cellX + 2, textY);

			console.write(value, bufferSize);
			if (autoFlush) {
				flush();
			}
		}

		return *this;
	}

	Spreadsheet& Spreadsheet::write(uint8_t cellId, const String& value) {
		return write(cellId, value.c_str(), value.length());
	}

	Spreadsheet& Spreadsheet::setGrid(uint8_t nbRows, uint8_t nbColumns) {
		this->nbRows = nbRows;
		this->nbColumns = nbColumns;

		enableDrawBorders(drawBorders);

		return *this;
	}

	Spreadsheet& Spreadsheet::flush() {
		console.getDisplay().flush();
		return *this;
	}

	Spreadsheet& Spreadsheet::enableAutoFlush(bool autoFlush) {
		this->autoFlush = autoFlush;
		return *this;
	}

	Spreadsheet& Spreadsheet::enableDrawBorders(bool drawBorders) {
		this->drawBorders = drawBorders;

		const Display& display = console.getDisplay();

		display.clear();

		if (drawBorders) {
			display.drawRectangle(0, 0, display.getWidth(), display.getHeight());

			//Draw intermediate vertical lines.
			float cellWidth = ((float) display.getWidth()) / nbColumns;
			for (uint8_t column = 1; column < nbColumns; column++) {
				display.drawVerticalLine(column * cellWidth, 0, display.getHeight());
			}

			//Draw intermediate horizontal lines.
			//Use float for precision and to share same metrics with "write" function that clears inside these cells.
			float cellHeight = ((float) display.getHeight()) / nbRows;
			for (uint8_t row = 1; row < nbRows; row++) {
				display.drawHorizontalLine(0, display.getWidth(), row * cellHeight);
			}
		}

		if (autoFlush) {
			display.flush();
		}

		return *this;
	}

	//------------------------------------------------------------------------//
	//--------------------------- Private methods ----------------------------//
	//------------------------------------------------------------------------//
} // namespace devduino
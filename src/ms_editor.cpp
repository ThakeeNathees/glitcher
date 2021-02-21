/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#include "ms_editor.h"

#include <Input.hpp>
#include <InputEventKey.hpp>
#include <InputEventMouse.hpp>
#include <InputEventMouseButton.hpp>
#include <InputEventMouseMotion.hpp>

typedef GlobalConstants GC;
#define IS_KEY_PRESSED(key) Input::get_singleton()->is_key_pressed(key)

const Vector2i Vector2i::ZERO = Vector2i(0, 0);

void MSEditor::_register_methods() {
  register_property<MSEditor, Ref<Font>>("font", &MSEditor::font, nullptr,
    GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_RESOURCE_TYPE);

  register_method("_ready", &MSEditor::_ready);
  register_method("_process", &MSEditor::_process);
  register_method("_draw", &MSEditor::_draw);
  register_method("_input", &MSEditor::_input);

  register_method("_on_blink_timeout", &MSEditor::_on_blink_timeout);

  register_method("get_text", &MSEditor::get_text);
  register_method("clear_errors", &MSEditor::clear_errors);
  register_method("set_line_error", &MSEditor::set_line_error);

  // TEMP.
  register_method("set_test_color", &MSEditor::set_test_color);
}

MSEditor::MSEditor() {
  lines.push_back("");

  highlighter.col_white = Color(1, 1, 1);
  highlighter.col_black = Color(0, 0, 0);
  highlighter.col_number = Color(0.92, 0.67, 0.39); // Color(1, 0.5, 0.31, 1);    // Orange.
  highlighter.col_string = Color(0.12, 0.80, 0.39);   // Green.
  highlighter.col_comment = Color(.45, .45, .45);     // Gray.
  highlighter.col_keyword = Color(1, .3, .5);         // Red.
  highlighter.col_function = Color(0.52, 0.64, 0.90); // Blue.

  highlighter.keywords.push_back("def");
  highlighter.keywords.push_back("native");
  highlighter.keywords.push_back("function");
  highlighter.keywords.push_back("end");
  highlighter.keywords.push_back("null");
  highlighter.keywords.push_back("self");
  highlighter.keywords.push_back("in");
  highlighter.keywords.push_back("and");
  highlighter.keywords.push_back("or");
  highlighter.keywords.push_back("not");
  highlighter.keywords.push_back("true");
  highlighter.keywords.push_back("false");
  highlighter.keywords.push_back("do");
  highlighter.keywords.push_back("while");
  highlighter.keywords.push_back("for");
  highlighter.keywords.push_back("if");
  highlighter.keywords.push_back("elif");
  highlighter.keywords.push_back("else");
  highlighter.keywords.push_back("break");
  highlighter.keywords.push_back("continue");
  highlighter.keywords.push_back("return");

  highlighter.builtin_fn.push_back("print");
  highlighter.builtin_fn.push_back("import");
  highlighter.builtin_fn.push_back("to_string");
  highlighter.builtin_fn.push_back("min");
  highlighter.builtin_fn.push_back("max");
}

MSEditor::~MSEditor() {
  // TODO: not like this -> blink_timer->free();
}

void MSEditor::_init() {
}

void MSEditor::_ready() {
  // Since the font is monospace it'll won't change with character.
  char_size = font.ptr()->get_string_size("A");
  draw_start = Vector2(char_size.x * 5, char_size.y * 4);

  blink_timer = Timer::_new();
  add_child(blink_timer);
  blink_timer->start(); // TODO: only start timer if has_focus and stop if not.
  blink_timer->set_wait_time(0.65);
  blink_timer->connect("timeout", this, "_on_blink_timeout");
}

void MSEditor::_process() {
}

String MSEditor::get_text() {
  std::stringstream ss;
  for (int i = 0; i < lines.size(); i++) {
    if (i > 0) ss << "\n";
    ss << lines[i];
  }
  return ss.str().c_str();
}

void MSEditor::clear_errors() {
  error_lines.clear();
}

void MSEditor::set_line_error(int line) {
  error_lines.insert(line-1); // error_lines are 0 based.
}

/*****************************************************************************
 * DRAW                                                                      *
 *****************************************************************************/

void MSEditor::_draw() {

#define _POS(_x, _y) (draw_start + Vector2((_x) * char_size.x, (_y) * char_size.y))

  // Draw the background. (FIXME: magic numbers)
  Color _col_bg = highlighter.col_black;
  Vector2 _area_pos = _POS(-4, -1);
  Vector2 _area_size = Vector2(char_size.x * line_width, char_size.y * line_height + font.ptr()->get_descent() + 2);
  draw_rect(Rect2(_area_pos, _area_size), _col_bg);

  // Selection region.
  Vector2i _start = select.get_first(); // Start of the selection.
  Vector2i _end = select.get_second();  // End of the selection.

  highlighter.highlight(lines);
  highlighter.start(first_line);
  Color _col_cursor = highlighter.col_white;

  for (int i = first_line; i < first_line + line_height; i++) {

    highlighter.newline();

// Convinent macros to map line coordinate to screen coordinate.
#define _POS_CHAR(_x, _y) _POS(_x, _y - first_line)
#define _POS_RECT(_x, _y) (_POS(_x, _y - first_line) - Vector2(0, font.ptr()->get_ascent()))

    // Draw line numbers.
    std::string _line_num_str; // 1 based number.
    Color _number_color;
    if (i < lines.size()) {
      _line_num_str = std::to_string(i + 1);
      _number_color = highlighter.col_number;
    } else {
      _line_num_str = "~";
      _number_color = highlighter.col_white;
    }
    Vector2 _line_num_offset = Vector2(2, 0);
    draw_string(font, _POS_CHAR(-((int)_line_num_str.size()), i) - _line_num_offset, _line_num_str.c_str(), _number_color);

    if (i >= lines.size()) continue;
    std::string& _line = lines[i];

    // Draw the line. (j <= _line.size() is because to draw selection on an empty line).
    for (int j = 0; j <= _line.size(); j++) {
      if (j == _line.size() && _line.size() != 0) continue;

      Color col_char = highlighter.col_white;
      if (_line.size() == 0 || j != _line.size()) {
        col_char = highlighter.get(); // Empty line has a color (for multi line string).
      }

      // Update cursor color.
      if (i == cursor.pos.y && j == cursor.pos.x) {
        _col_cursor = col_char;
      }

      bool is_selected = false;
      if (select.is_active()) {
        if (i == _start.y) {
          if (i == _end.y) {
            // Selection start and end are the same line.
            if (_start.x <= j && j <= _end.x) {
              is_selected = true;
            }
          } else if (_start.x <= j) {
            is_selected = true;
          }
        } else if (i == _end.y) {
          if (j <= _end.x) {
            is_selected = true;
          }
        } else if (_start.y < i && i < _end.y) {
            is_selected = true;
        }

        // Draw the selection.
        if (is_selected) {
          draw_rect(Rect2(_POS_RECT(j, i), char_size), col_char);
          col_char = _col_bg;
        }
      }
      if (j == _line.size()) continue;

      String c = _line[j];
      if (is_selected) col_char = Color(0, 0, 0);
      draw_char(font, _POS_CHAR(j, i), c, "", col_char);
    }
  }

  if (cursor_visible) {
    // Draw cursor. (cursor size = char_size)
    Vector2 _cursor_pos = _POS_RECT(cursor.pos.x, cursor.pos.y);
    draw_rect(Rect2(_cursor_pos, char_size), _col_cursor);

    // Draw the char hovered by the cursor.
    if (cursor.pos.x < lines[cursor.pos.y].size()) {
      String c = lines[cursor.pos.y][cursor.pos.x];
      Vector2 _char_pos = _POS_CHAR(cursor.pos.x, cursor.pos.y);
      draw_char(font, _char_pos, c, "", _col_bg);
    }
  }

}

/*****************************************************************************
 * INPUT                                                                     *
 *****************************************************************************/

// Forward declarations of methods.
static std::pair<std::string, int> key_to_string(InputEventKey* event,
  char next_char);

void MSEditor::_input(Variant event) {
  Ref<InputEventKey> key_event = event;
  if (key_event.is_valid() && key_event.ptr()->is_pressed()) {

    int64_t key = key_event.ptr()->get_scancode();
    const std::string& _line = lines[cursor.pos.y];

    char _next_char = 0;
    if (cursor.pos.x < _line.size()) _next_char = _line[cursor.pos.x];
    std::pair<std::string, int> _printable = key_to_string(key_event.ptr(), _next_char);

    if (_printable.first != "") {
      if (select.is_active()) clear_selected();
      lines[cursor.pos.y].insert(cursor.pos.x, _printable.first);
      cursor.pos.x += _printable.second;
      reset_blink_timer();

    } else if (key == GC::KEY_BACKSPACE) {
      handle_backspace();
      reset_blink_timer();

    } else if (key == GC::KEY_ENTER) {
      if (select.is_active()) clear_selected();

      // Get indentation.
      std::string _line = lines[cursor.pos.y];
      int indentation = 0;
      for (int i = 0; i < _line.size(); i++) {
        if (_line[i] != ' ') break;
        indentation++;
      }

      // Remove everything after the cursor pos.
      lines[cursor.pos.y] = _line.substr(0, cursor.pos.x);

      // Add a new line.
      lines.insert(lines.begin() + cursor.pos.y + 1,
        _line.substr(0, indentation) + 
        ((cursor.pos.x < _line.size()) ? _line.substr(cursor.pos.x) : ""));
      cursor.pos.y += 1;
      cursor.pos.x = indentation;

      ensure_cursor_visible();
      reset_blink_timer();

    } else if (is_key_navigation(key)) {
      handle_navigation(key);
      reset_blink_timer();

    } else {
      // TODO: unhandled key.
      // reset_blink_timer();
    }


    // -----------------------------------------------------
    if (key != GC::KEY_UP && key != GC::KEY_DOWN) {
      cursor.intended = cursor.pos.x;
    }

    // Call _draw() method.
    update();
  }

  Ref<InputEventMouseMotion> mouse_button_event = event;
  if (mouse_button_event.is_valid()) {
    // TODO:
    // update();
  }
}


// If key is printable char or more (string) it'll return it and return the 
// number of columns cursor has to increase.
static std::pair<std::string, int> key_to_string(InputEventKey* event,
  char next_char) {
  int64_t key = event->get_scancode();
  int64_t code = event->get_unicode();
  if (key == GC::KEY_TAB) {
    return { "  ", 2 }; // Tab is 2 spaces.
  }
  if (32 <= code && code < 127) {
    // TODO: pair symbol
    return { std::string(1, (char)code), 1 };
  }
  return { "", 0 };
}

void MSEditor::clear_selected() {
  if (!select.is_active()) return;
  Vector2i _start = select.get_first();
  Vector2i _end = select.get_second();

  if (_start.y == _end.y) {
    std::string _line = lines[_start.y];
    lines[_start.y] = _line.substr(0, _start.x) + _line.substr(_end.x);
  } else {
    lines[_start.y] = lines[_start.y].substr(0, _start.x) + lines[_end.y].substr(_end.x);
    for (int i = _start.y + 1; i <= _end.y; i++) {
      lines.erase(lines.begin() + _start.y + 1);
    }
  }

  cursor.pos = _start;
  select.clear();
}


// It'll return the next position if navigate or backspaced with ctrl.
// And use that position to set the cursor or remove chars.
static int ctrl_move_pos(const std::string& line, int curr, bool left) {
  int _pos = curr; // After moved.

  if (left) {
    // Skip white spaces.
    while (_pos > 0 && line[_pos - 1] == ' ') {
      _pos -= 1;
    }

    // Skip a word.
    while (_pos > 0) {
      char c = line[_pos - 1];
      if (IS_NAME(c)) _pos -= 1;
      else break;
    }

    if (_pos == curr && _pos > 0)  return _pos - 1;

  } else { // Right.

    while (_pos < line.size() && line[_pos] == ' ') {
      _pos += 1;
    }

    // If space consumed moving right we're done.
    if (_pos != curr) return _pos;

    while (_pos < line.size()) {
      char c = line[_pos];
      if (IS_NAME(c)) _pos += 1;
      else break;
    }

    while (_pos < line.size() && line[_pos] == ' ') {
      _pos += 1;
    }

    if (_pos == curr && _pos < line.size()) return _pos + 1;
  }
  
  return _pos;
}

void MSEditor::handle_backspace() {
  if (select.is_active()) {
    clear_selected();
    ensure_cursor_visible();
    return;
  }

  if (cursor.pos.x > 0) { // Erase the last char.
    std::string& _line = lines[cursor.pos.y];

    int chars_to_remove = 1; // Number of chars to remove.
    if (IS_KEY_PRESSED(GC::KEY_CONTROL)) {
      int _pos = ctrl_move_pos(_line, cursor.pos.x, true);
      chars_to_remove = std::max(1, cursor.pos.x - _pos);
    }

    _line = _line.substr(0, (int)(cursor.pos.x - chars_to_remove)) + _line.substr(cursor.pos.x);
    cursor.pos.x -= chars_to_remove;

  } else if (cursor.pos.y > 0) { // Erase the new line.
    std::string _line = lines[cursor.pos.y];
    lines.erase(lines.begin() + cursor.pos.y);
    cursor.pos.y -= 1;
    cursor.pos.x = (int)lines[cursor.pos.y].size();
    lines[cursor.pos.y] += _line;
  }
  ensure_cursor_visible();
}

bool MSEditor::is_key_navigation(int64_t key) {
  return (key == GC::KEY_UP) ||
    (key == GC::KEY_DOWN) ||
    (key == GC::KEY_LEFT) ||
    (key == GC::KEY_RIGHT) ||
    (key == GC::KEY_HOME) ||
    (key == GC::KEY_END);
}

void MSEditor::handle_navigation(int64_t key) {

  Vector2i cursor_last = cursor.pos;

  switch (key) {

    case GC::KEY_UP: {
      if (cursor.pos.y == 0) {
        cursor.pos.x = 0;
        cursor.intended = 0;
      } else {
        if (IS_KEY_PRESSED(GC::KEY_ALT)) {
          // TODO:
        } else {
          cursor.pos.y -= 1;
          cursor.pos.x = std::min(cursor.intended, (int)lines[cursor.pos.y].size());
        }
      }
    } break;


    case GC::KEY_DOWN: {
      if (cursor.pos.y == lines.size() - 1) {
        cursor.pos.x = (int)lines[lines.size() - 1].size();
        cursor.intended = cursor.pos.x;
      } else {
        if (IS_KEY_PRESSED(GC::KEY_ALT)) {
          // TODO:
        } else {
          cursor.pos.y += 1;
          cursor.pos.x = std::min(cursor.intended, (int)lines[cursor.pos.y].size());
        }
      }
    } break;


    case GC::KEY_LEFT: {
      if (select.is_active() && !IS_KEY_PRESSED(GC::KEY_SHIFT)) {
        cursor.pos = select.get_first();
        select.clear();
        break;
      }
      if (cursor.pos.x == 0) {
        if (cursor.pos.y > 0) {
          cursor.pos.y -= 1;
          cursor.pos.x = (int)lines[cursor.pos.y].size();
        }
      } else {
        if (IS_KEY_PRESSED(GC::KEY_CONTROL))
          cursor.pos.x = ctrl_move_pos(lines[cursor.pos.y], cursor.pos.x, true);
        else cursor.pos.x -= 1;
      }
    } break;


    case GC::KEY_RIGHT: {
      if (select.is_active() && !IS_KEY_PRESSED(GC::KEY_SHIFT)) {
        cursor.pos = select.get_second();
        select.clear();
        break;
      }
      if (cursor.pos.x == lines[cursor.pos.y].size()) {
        if (cursor.pos.y + 1 < (int)lines.size()) {
          cursor.pos.y += 1;
          cursor.pos.x = 0;
        }
      } else {
        if (IS_KEY_PRESSED(GC::KEY_CONTROL))
          cursor.pos.x = ctrl_move_pos(lines[cursor.pos.y], cursor.pos.x, false);
        else cursor.pos.x += 1;
      }
    } break;

    case GC::KEY_HOME: {
      if (IS_KEY_PRESSED(GC::KEY_CONTROL)) {
        cursor.pos.x = 0;
        cursor.pos.y = 0;
        break;
      }

      int indent = 0;
      for (char c : lines[cursor.pos.y]) {
        if (c != ' ') break;
        indent += 1;
      }
      if (cursor.pos.x > indent || cursor.pos.x == 0) {
        cursor.pos.x = indent;
      } else {
        cursor.pos.x = 0;
      }
    } break;

    case GC::KEY_END: {
      if (IS_KEY_PRESSED(GC::KEY_CONTROL)) {
        cursor.pos.y = (int)lines.size() - 1;
        cursor.pos.x = (int)lines[cursor.pos.y].length();
        break;
      }

      cursor.pos.x = (int)lines[cursor.pos.y].size();
    } break;

    default:
      UNREACHABLE();
  }

  if (IS_KEY_PRESSED(GC::KEY_SHIFT)) {
    if (!select.is_active()) {
      select.start = cursor_last;
    }
    select.end = cursor.pos;
  } else {
    select.clear();
  }
  ensure_cursor_visible();

}


void MSEditor::ensure_cursor_visible() {

  // Curser is far below.
  if (cursor.pos.y - first_line + 1 > line_height) {
    first_line = cursor.pos.y - line_height + 1;
  }

  // Cursor is far above.
  if (cursor.pos.y < first_line) {
    first_line = cursor.pos.y;
  }
}

void MSEditor::reset_blink_timer() {
  // TODO: if (visible and has_focus) reset otherwise return.
  cursor_visible = true;
  blink_timer->stop();
  blink_timer->start();
}

/*****************************************************************************
 * SYNTAX HIGHLIGHTING                                                       *
 *****************************************************************************/

void SyntaxHighlight::start(int first_line) {
  curr_line = first_line - 1; // newline() imediatly follows.
  curr_col = 0;
  index = 0;
}

void SyntaxHighlight::newline() {
  curr_line += 1;
  curr_col = 0;
  index = 0;
}

Color SyntaxHighlight::get() {
  if (data[curr_line][index].second < curr_col) {
    index++;
  }
  curr_col++;
  return data[curr_line][index].first;
}

void SyntaxHighlight::highlight(std::vector<std::string>& lines) {
  data.clear();

  // Numbers, strings, comment, symbols, keywords, attrib, call, just name.

  bool in_string = false;
  bool double_quote = false; // String type.

  for (std::string& line : lines) {

    data.push_back({});
    std::vector<std::pair<Color, int>>& line_colors = data[data.size() - 1];

    if (line.size() == 0) { // Empty line.
      if (in_string) {
        line_colors.push_back({col_string, 0});
      } else {
        line_colors.push_back({ col_white, 0 });
      }
      continue;
    }

    int i = 0;
    while (i < line.size()) {

      // If already in string end it.
      if (in_string) {
        while (i < line.size()) {
          char c = line[i];
          // TODO: handle_escape();
          if (double_quote && c == '"') {
            in_string = false;
            line_colors.push_back({ col_string, i++ });
            break;
          }
          if (!double_quote && c == '\'') {
            in_string = false;
            line_colors.push_back({ col_string, i++ });
            break;
          }
          i++;
        }
        if (/*still*/in_string) { // Complete line is inside the string.
          line_colors.push_back({ col_string, i++ });
        }
        continue;
      }

      char c = line[i];
      switch (c) {
        
        case '\'':
        case '"': {
          ASSERT(!in_string, OOPS);

          in_string = true;
          double_quote = c == '"';
          
          if (i + 1 == line.size()) {
            line_colors.push_back({ col_string, i++ });
          
          } else {
            i++;
            bool excape_next = false;
            while (i < line.size()) {
              if (line[i] == c) { // check end quote.
                in_string = false;
                break;
              }
              // TODO: escape char.
              i++;
            }
            line_colors.push_back({ col_string, i++ }); // i++ will consume end quote.
          }
        } break;

        case '#': { // Hard coded for miniscript.
          i = (int)line.size();
          line_colors.push_back({ col_comment, i });
        } break;

        case ' ':
        case '\t': {
          while (i < line.size()) {
            if (line[i] != ' ' && line[i] != '\t') break;
            i++;
          }
          line_colors.push_back({ col_white, i - 1 });
        } break;

         //TODO:
        default:
        {
          // Number.
          if ('0' <= c && c <= '9') {
            bool _decimal = false; // True if decimal point found.
            while (i < line.size()) {
              c = line[i];
              if (c != '.' && (c < '0' || '9' < c)) break;
              if (_decimal && c == '.') break;

              if (c == '.') {
                // Range syntax is 0..10 so don't consume 2 dots.
                if (i <= line.size() - 2 && line[i + 1] == '.') break;
                _decimal = true;
              }
              i++;
            }
            line_colors.push_back({ col_number, i - 1 });
            break;
          }

          // Word.
          if (IS_NAME(c)) {
            std::string word;
            int j = i;
            while (j < line.size()) {
              if (!IS_NAME(line[j])) {
                break;
              }
              j++;
            }
            word = line.substr(i, j - i);
            i = j - 1; // i is at the last letter of the word.
          
            bool _is_colored = false; // True if done with the word.
            for (std::string& kw : keywords) {
              if (word == kw) {
                line_colors.push_back({ col_keyword, i++ });
                _is_colored = true;
                break;
              }
            }
            if (_is_colored) break;

            for (std::string& kw : builtin_fn) {
              if (word == kw) {
                line_colors.push_back({ col_function, i++ });
                _is_colored = true;
                break;
              }
            }
            if (_is_colored) break;


            // Other words.
            line_colors.push_back({ col_white, i++ });

            break;
          }

          // TODO:

          line_colors.push_back({ col_white, i++ });

        } break;
      }
    }

  }
}

/*****************************************************************************/
/* SIGNAL RECIVERS                                                           */
/*****************************************************************************/

void MSEditor::_on_blink_timeout() {
  // TODO: is_visible and has_focus -> update()
  cursor_visible = !cursor_visible;
  update();
}
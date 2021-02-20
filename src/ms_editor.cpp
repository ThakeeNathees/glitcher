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
}

void MSEditor::_init() {
  lines.push_back("");
}

void MSEditor::_ready() {
  // Since the font is monospace it'll won't change with character.
  char_size = font.ptr()->get_string_size("A");
  draw_start = Vector2(char_size.x * 4, char_size.y * 4);
}

void MSEditor::_process() {
  update();
}

/*****************************************************************************
 * DRAW                                                                      *
 *****************************************************************************/

void MSEditor::_draw() {

  Vector2 _pos;
  Vector2i _start = select.get_first();
  Vector2i _end = select.get_second();


  // Draw the background.
  Vector2 _area_pos = draw_start - Vector2( char_size.x * 3, char_size.y);
  Vector2 _area_size = Vector2(char_size.x * 80, char_size.y * line_height + 4); // TODO: 80 chars per line. +4 space.
  draw_rect(Rect2(_area_pos, _area_size), Color(0, 0, 0, 1));

  for (int i = first_line; i < std::min(first_line + line_height, (int)lines.size()); i++) {
    std::string& _line = lines[i];

    _pos = draw_start + Vector2(0, (i - first_line) * char_size.y);

    // Draw line numbers.
    std::string line_num_str = std::to_string(i + 1); // 1 based number.
    float line_num_offset = line_num_str.size() * char_size.x + 2; //<-- TODO: 2 is another magic number line spaceing.
    draw_string(font, _pos - Vector2(line_num_offset, 0), line_num_str.c_str(), Color(1, 0.5, 0.31, 1));

    // Draw selection.
    if (select.is_active()) {
      if (_start.y <= i && i <= _end.y) {
        Vector2 _select_pos = _pos - Vector2(0, font.ptr()->get_ascent());
        Vector2 _select_size = Vector2(_line.size() * char_size.x, char_size.y);
        _select_size.x = std::max(_select_size.x, char_size.x);

        if (i == _start.y) { // start x position.
          real_t _offset = char_size.x * _start.x;
          _select_pos += Vector2(_offset, (real_t)0);
          _select_size.x -= _offset;
        }

        if (i == _end.y) {
          real_t _offset = ((real_t)_line.size() - _end.x) * char_size.x;
          _select_size.x -= _offset;
        }

        draw_rect(Rect2(_select_pos, _select_size), Color(0, 1, 1, 1));
      }
    }

    // Draw the line.
    for (int j = 0; j < _line.size(); j++) {
      String c = _line[j];
      draw_char(font, _pos + Vector2(j * char_size.x, 0), c, "");
    }
    //draw_string(font, _pos, _line.c_str());
  }


  // Draw cursor. (cursor size = char_size)
  _pos = draw_start - Vector2(0, font.ptr()->get_ascent());
  _pos.x += cursor.pos.x * char_size.x;
  _pos.y += (cursor.pos.y - first_line) * char_size.y;
  draw_rect(Rect2(_pos, char_size), Color(1, 1, 1, 1));
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

    } else if (key == GC::KEY_BACKSPACE) {
      handle_backspace();

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

    } else if (is_key_navigation(key)) {
      handle_navigation(key);

    } else {
      // TODO: unhandled key.
    }


    // -----------------------------------------------------
    if (key != GC::KEY_UP && key != GC::KEY_DOWN) {
      cursor.intended = cursor.pos.x;
    }

  }

  Ref<InputEventMouseMotion> mouse_button_event = event;
  if (mouse_button_event.is_valid()) {
    // TODO:
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
      if (c == '_' || isalpha(c)) _pos -= 1;
      else break;
    }

  } else {

    while (_pos < line.size() && line[_pos] == ' ') {
      _pos += 1;
    }

    // If space consumed moving right we're done.
    if (_pos != curr) return _pos;

    while (_pos < line.size()) {
      char c = line[_pos];
      if (c == '_' || isalpha(c)) _pos += 1;
      else break;
    }

    while (_pos < line.size() && line[_pos] == ' ') {
      _pos += 1;
    }
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

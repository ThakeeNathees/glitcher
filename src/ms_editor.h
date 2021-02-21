/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef MS_EDITOR
#define MS_EDITOR

#include "common.h"
#include "types.h"
#include <Node2D.hpp>

#include <Font.hpp>
#include <InputEvent.hpp>
#include <Timer.hpp>


class MSEditor : public Node2D {
  GODOT_CLASS(MSEditor, Node2D);

  Ref<Font> font; // Current font.
  std::vector<std::string> lines; // Array of lines.
  SyntaxHighlight highlighter;

  Cursor cursor;
  bool cursor_visible = true; // Change during the blink.
  Timer* blink_timer = nullptr;

  Selection select;
  Vector2 draw_start;
  Vector2 char_size;

  int line_height = 20;  // Number of rows at once visible.
  int line_width = 80;   // Max number of chars per line.
  int first_line = 0;    // First visible line number 0 based.

  std::set<int> error_lines; // Lines marked with Red.

public:
  static void _register_methods();

  MSEditor();
  ~MSEditor();

  void _init();
  void _ready();
  void _process();
  void _draw();
  void _input(Variant event);

  void _on_blink_timeout();

  String get_text();
  void clear_errors();
  void set_line_error(int line); // [line] must be 1 based.

  // TEMP.
  void set_test_color(Color col) {
    highlighter._col_test = col;
    update();
  }

private:
  void clear_selected();
  void handle_backspace();
  void handle_navigation(int64_t key);
  bool is_key_navigation(int64_t key);

  void ensure_cursor_visible();
  void reset_blink_timer();

};



#endif // MS_EDITOR

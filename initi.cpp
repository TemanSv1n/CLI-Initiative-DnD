#include <ncurses.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <sstream>

using namespace std;

class TableRenderer {
private:
    vector<string> names //= {"Hitler", "Goonner", "Pedik"}
    ;
    vector<int> inis //= {10, 14, 93}
    ;
    vector<int> hits //= {14, 10, 13}
    ;
    int selected_row = 0;
    int max_name_width = 10;
    int table_width, table_x;
    bool move_mode = false;
    int move_original_pos = -1;

    void sortGoons() {
        for (size_t i = 0; i < inis.size(); ++i) {
            for (size_t j = 0; j < inis.size() - i - 1; ++j) {
                if (inis[j] < inis[j+1]) { 
                    swap(inis[j], inis[j+1]);
                    swap(names[j], names[j+1]);
                    swap(hits[j], hits[j+1]);   
                }
            }
        }
    }

    void deleteEntry(size_t index) {
        if (index >= names.size()) return;
        names.erase(names.begin() + index);
        inis.erase(inis.begin() + index);
        hits.erase(hits.begin() + index);
    }

    void addEntry(string name, int iniss, int hit = -1488) {
        names.push_back(name);
        inis.push_back(iniss);
        hits.push_back(hit);
        sortGoons();
    }

    void moveEntry(int from, int to) {
        if (from < 0 || from >= names.size() || to < 0 || to >= names.size()) return;
        
        // Save the entry we're moving
        string name = names[from];
        int ini = inis[from];
        int hit = hits[from];
        
        // Remove from original position
        names.erase(names.begin() + from);
        inis.erase(inis.begin() + from);
        hits.erase(hits.begin() + from);
        
        // Insert at new position
        names.insert(names.begin() + to, name);
        inis.insert(inis.begin() + to, ini);
        hits.insert(hits.begin() + to, hit);
        
        selected_row = to;
    }

    void calculate_layout() {
        max_name_width = 10;
        for (const auto& name : names) {
            max_name_width = max(max_name_width, static_cast<int>(name.length()));
        }
        table_width = 8 + max_name_width + 8 + 8;
        int max_x = getmaxx(stdscr);
        table_x = (max_x - table_width) / 2;
        if (table_x < 0) table_x = 0;
    }

    void draw_box(int y, int width, int height) {
        mvaddch(y, table_x, ACS_ULCORNER);
        mvaddch(y, table_x + width, ACS_URCORNER);
        mvaddch(y + height, table_x, ACS_LLCORNER);
        mvaddch(y + height, table_x + width, ACS_LRCORNER);

        for (int i = 1; i < width; i++) {
            mvaddch(y, table_x + i, ACS_HLINE);
            mvaddch(y + height, table_x + i, ACS_HLINE);
        }

        for (int i = 1; i < height; i++) {
            mvaddch(y + i, table_x, ACS_VLINE);
            mvaddch(y + i, table_x + width, ACS_VLINE);
        }
    }

    void draw_horizontal_line(int y) {
        int max_x = getmaxx(stdscr);
        for (int x = 0; x < max_x; x++) {
            mvaddch(y, x, ACS_HLINE);
        }
    }

    void draw_header() {
        attron(A_REVERSE);
        mvprintw(2, table_x + 2, " # ");
        mvprintw(2, table_x + 6, " %-*s ", max_name_width, "Name");
        mvprintw(2, table_x + 8 + max_name_width, " Ini ");
        mvprintw(2, table_x + 14 + max_name_width, " Hits ");
        attroff(A_REVERSE);
    }

    void draw_rows() {
        for (size_t i = 0; i < names.size(); i++) {
            int y = i + 3;
            if (i == static_cast<size_t>(selected_row)) {
                if (move_mode) {
                    attron(A_STANDOUT);  // Different highlight for move mode
                } else {
                    attron(A_REVERSE);
                }
            }
            mvprintw(y, table_x + 2, " %zu ", i);
            mvprintw(y, table_x + 6, " %-*s ", max_name_width, names[i].c_str());
            mvprintw(y, table_x + 8 + max_name_width, " %3d ", inis[i]);
            
            if (hits[i] != -1488) {
                mvprintw(y, table_x + 14 + max_name_width, " %3d ", hits[i]);
            } else {
                mvprintw(y, table_x + 14 + max_name_width, "     ");
            }
            
            if (i == static_cast<size_t>(selected_row)) {
                if (move_mode) {
                    attroff(A_STANDOUT);
                } else {
                    attroff(A_REVERSE);
                }
            }
        }
    }

    void draw_buttons() {
        int max_y = getmaxy(stdscr);
        int buttons_y = max_y - 1;
        draw_horizontal_line(buttons_y - 2);
        
        int button_spacing = 2;
        int buttons_x = (getmaxx(stdscr) - (60 + 5*button_spacing)) / 2;  // Increased width for new button
        
        attron(A_REVERSE);
        if (move_mode) {
            mvprintw(buttons_y, buttons_x, " [Enter] Confirm ");
            buttons_x += 18 + button_spacing;
            mvprintw(buttons_y, buttons_x, " [ESC] Cancel ");
        } else {
            mvprintw(buttons_y, buttons_x, " [A] Add ");
            buttons_x += 10 + button_spacing;
            mvprintw(buttons_y, buttons_x, " [E] Edit ");
            buttons_x += 10 + button_spacing;
            mvprintw(buttons_y, buttons_x, " [D] Delete ");
            buttons_x += 12 + button_spacing;
            mvprintw(buttons_y, buttons_x, " [H] Hit ");
            buttons_x += 10 + button_spacing;
            mvprintw(buttons_y, buttons_x, " [M] Move ");
            buttons_x += 10 + button_spacing;
            mvprintw(buttons_y, buttons_x, " [Q] Quit ");
        }
        attroff(A_REVERSE);
    }

    void show_add_dialog() {
        echo();
        curs_set(1);
        
        int width = 30;
        int height = 6;
        int x = (getmaxx(stdscr) - width) / 2;
        int y = (getmaxy(stdscr) - height) / 2;
        
        // Draw box
        mvaddch(y, x, ACS_ULCORNER);
        mvaddch(y, x + width, ACS_URCORNER);
        mvaddch(y + height, x, ACS_LLCORNER);
        mvaddch(y + height, x + width, ACS_LRCORNER);
        for (int i = 1; i < width; i++) {
            mvaddch(y, x + i, ACS_HLINE);
            mvaddch(y + height, x + i, ACS_HLINE);
        }
        for (int i = 1; i < height; i++) {
            mvaddch(y + i, x, ACS_VLINE);
            mvaddch(y + i, x + width, ACS_VLINE);
        }
        
        mvprintw(y + 1, x + 2, "Add New Entry:");
        mvprintw(y + 2, x + 2, "Name: ");
        mvprintw(y + 3, x + 2, "Initiative: ");
        mvprintw(y + 4, x + 2, "Hits (optional): ");
        
        char name[256];
        mvgetnstr(y + 2, x + 8, name, sizeof(name) - 1);
        
        char ini_str[256];
        mvgetnstr(y + 3, x + 14, ini_str, sizeof(ini_str) - 1);
        int ini = atoi(ini_str);
        
        char hits_str[256];
        mvgetnstr(y + 4, x + 18, hits_str, sizeof(hits_str) - 1);
        int hits_val = -1488;
        if (strlen(hits_str) > 0) {
            hits_val = atoi(hits_str);
        }
        
        noecho();
        curs_set(0);
        
        if (strlen(name) > 0 && ini >= 0) {
            addEntry(name, ini, hits_val);
        }
    }

    void show_edit_dialog() {
        if (names.empty()) return;

        // Save original terminal settings
        bool original_echo = is_echo();
        int original_cursor = curs_set(1);
        
        // Dialog setup
        int width = 30;
        int height = 6;
        int x = (getmaxx(stdscr) - width) / 2;
        int y = (getmaxy(stdscr) - height) / 2;
        
        // Draw box
        box(stdscr, 0, 0);
        mvaddch(y, x, ACS_ULCORNER);
        mvaddch(y, x + width, ACS_URCORNER);
        mvaddch(y + height, x, ACS_LLCORNER);
        mvaddch(y + height, x + width, ACS_LRCORNER);
        
        // Field positions
        const int name_x = x + 8;
        const int ini_x = x + 14;
        const int hits_x = x + 8;
        
        // Current values
        string fields[3] = {
            names[selected_row],
            to_string(inis[selected_row]),
            hits[selected_row] == -1488 ? "" : to_string(hits[selected_row])
        };
        
        int current_field = 0;
        bool editing = true;
        
        while (editing) {
            // Clear fields
            for (int i = 0; i < 3; i++) {
                mvprintw(y + 2 + i, x + 2, "%-*s", width - 4, "");
            }
            
            // Draw labels and fields
            mvprintw(y + 1, x + 2, "Edit Entry:");
            mvprintw(y + 2, x + 2, "Name: ");
            mvprintw(y + 3, x + 2, "Initiative: ");
            mvprintw(y + 4, x + 2, "Hits: ");
            
            // Show field values
            mvprintw(y + 2, name_x, "%-*s", max_name_width, fields[0].c_str());
            mvprintw(y + 3, ini_x, "%-3s", fields[1].c_str());
            mvprintw(y + 4, hits_x, "%-3s", fields[2].c_str());
            
            // Position cursor
            int cursor_x = 0;
            switch (current_field) {
                case 0: cursor_x = name_x + fields[0].length(); break;
                case 1: cursor_x = ini_x + fields[1].length(); break;
                case 2: cursor_x = hits_x + fields[2].length(); break;
            }
            move(y + 2 + current_field, min(cursor_x, x + width - 2));
            refresh();
            
            // Get input
            int ch = getch();
            switch (ch) {
                case KEY_UP:
                    current_field = max(0, current_field - 1);
                    break;
                case KEY_DOWN:
                    current_field = min(2, current_field + 1);
                    break;
                case '\n': // Enter
                    editing = false;
                    break;
                case 27: // ESC
                    editing = false;
                    // Restore settings and return without saving
                    if (!original_echo) noecho();
                    curs_set(original_cursor);
                    return;
                case KEY_BACKSPACE:
                case 127:
                    if (!fields[current_field].empty()) {
                        fields[current_field].pop_back();
                    }
                    break;
                default:
                    if (current_field == 0) { // Name
                        if (isprint(ch) && fields[0].length() < max_name_width*2) {
                            fields[0] += ch;
                        }
                    } 
                    else if (current_field == 1) { // Initiative
                        if (isdigit(ch) && fields[1].length() < 7) {
                            fields[1] += ch;
                        }
                    }
                    else if (current_field == 2) { // Hits
                        if ((isdigit(ch) || (ch == '-' && fields[2].empty())) && 
                            fields[2].length() < 7) {
                            fields[2] += ch;
                        }
                    }
                    break;
            }
        }
        
        // Save changes
        if (!fields[0].empty()) {
            names[selected_row] = fields[0];
            inis[selected_row] = fields[1].empty() ? 0 : stoi(fields[1]);
            hits[selected_row] = fields[2].empty() ? -1488 : stoi(fields[2]);
            sortGoons();
        }
        
        // Restore terminal settings
        if (!original_echo) noecho();
        curs_set(original_cursor);
    }

    void show_hit_dialog() {
        if (names.empty() || selected_row < 0 || selected_row >= names.size()) return;
        
        echo();
        curs_set(1);
        
        int width = 30;
        int height = 4;
        int x = (getmaxx(stdscr) - width) / 2;
        int y = (getmaxy(stdscr) - height) / 2;
        
        // Draw box
        mvaddch(y, x, ACS_ULCORNER);
        mvaddch(y, x + width, ACS_URCORNER);
        mvaddch(y + height, x, ACS_LLCORNER);
        mvaddch(y + height, x + width, ACS_LRCORNER);
        for (int i = 1; i < width; i++) {
            mvaddch(y, x + i, ACS_HLINE);
            mvaddch(y + height, x + i, ACS_HLINE);
        }
        for (int i = 1; i < height; i++) {
            mvaddch(y + i, x, ACS_VLINE);
            mvaddch(y + i, x + width, ACS_VLINE);
        }
        
        mvprintw(y + 1, x + 2, "Modify hits for %s:", names[selected_row].c_str());
        mvprintw(y + 2, x + 2, "Hits: ");
        
        char hit_str[256];
        mvgetnstr(y + 2, x + 8, hit_str, sizeof(hit_str) - 1);  // Changed from x+38 to x+8
        
        noecho();
        curs_set(0);
        
        if (strlen(hit_str) > 0) {
            int hit_change = atoi(hit_str);
            if (hits[selected_row] == -1488) {
                hits[selected_row] = hit_change;
            } else {
                hits[selected_row] += hit_change;
            }
            }
    }

    void start_move_mode() {
        if (names.empty()) return;
        move_mode = true;
        move_original_pos = selected_row;
    }

    void end_move_mode(bool confirm) {
        move_mode = false;
        if (!confirm) {
            selected_row = move_original_pos;
        } else {
            sortGoons();
        }
        move_original_pos = -1;
    }

    void show_dummy_dialog(const string& action) {
        int width = 30;
        int height = 3;
        int x = (getmaxx(stdscr) - width) / 2;
        int y = (getmaxy(stdscr) - height) / 2;
        
        mvaddch(y, x, ACS_ULCORNER);
        mvaddch(y, x + width, ACS_URCORNER);
        mvaddch(y + height, x, ACS_LLCORNER);
        mvaddch(y + height, x + width, ACS_LRCORNER);
        for (int i = 1; i < width; i++) {
            mvaddch(y, x + i, ACS_HLINE);
            mvaddch(y + height, x + i, ACS_HLINE);
        }
        for (int i = 1; i < height; i++) {
            mvaddch(y + i, x, ACS_VLINE);
            mvaddch(y + i, x + width, ACS_VLINE);
        }
        
        mvprintw(y + 1, x + 2, "%s functionality", action.c_str());
        mvprintw(y + 2, x + 2, "Press any key...");
        refresh();
        getch();
    }

public:
    void refresh_display() {
        clear();
        calculate_layout();
        draw_box(1, table_width, names.size() + 2);
        draw_header();
        draw_rows();
        draw_buttons();
        refresh();
    }

    void run() {
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(0);

        refresh_display();

        int ch;
        while ((ch = getch()) != 'q' && ch != 'Q') {
            if (move_mode) {
                switch(ch) {
                    case KEY_UP:
                        if (selected_row > 0) {
                            moveEntry(selected_row, selected_row - 1);
                        }
                        break;
                    case KEY_DOWN:
                        if (selected_row < static_cast<int>(names.size()) - 1) {
                            moveEntry(selected_row, selected_row + 1);
                        }
                        break;
                    case '\n': // Enter
                        end_move_mode(true);
                        break;
                    case 27: // ESC
                        end_move_mode(false);
                        break;
                }
            } else {
                switch(ch) {
                    case KEY_UP: 
                        if (selected_row > 0) selected_row--;
                        break;
                    case KEY_DOWN:
                        if (selected_row < static_cast<int>(names.size()) - 1) selected_row++;
                        break;
                    case 'a': case 'A':
                        show_add_dialog();
                        break;
                    case 'e': case 'E':
                        show_edit_dialog();
                        break;
                    case 'd': case 'D':
                        if (names.size() > 0) {
                            deleteEntry(selected_row);
                            if (selected_row >= names.size()) selected_row = names.size() - 1;
                        }
                        break;
                    case 'h': case 'H':
                        show_hit_dialog();
                        break;
                    case 'm': case 'M':
                        start_move_mode();
                        break;
                }
            }
            refresh_display();
        }
        endwin();
    }
};

int main() {
    TableRenderer renderer;
    renderer.run();
    return 0;
}
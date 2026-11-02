#define mute_button(plug, boundary) \
    mute_button_with_location((plug), __FILE__, __LINE__, (boundary))
static bool mute_button_with_location(Plug *plug, const char *file, int line, Rectangle boundary)
{
    static float saved_volume = 0.5f;

    float icon_size = 512;
    float scale = HUD_BUTTON_SIZE/icon_size*HUD_ICON_SCALE;
    Rectangle dest = {
        boundary.x + HUD_BUTTON_SIZE/2 - icon_size*scale/2,
        boundary.y + HUD_BUTTON_SIZE/2 - icon_size*scale/2,
        icon_size*scale,
        icon_size*scale
    };

    float volume = GetMasterVolume();
    size_t icon_index;
    if (volume <= 0) {
        icon_index = 0;
    } else {
        size_t phases = 2;
        icon_index = volume*phases;
        if (icon_index >= phases) icon_index = phases - 1;
        icon_index += 1;
    }

    Rectangle source = {icon_size*icon_index, 0, icon_size, icon_size};
    DrawTexturePro(plug->icon_textures[UI_ICON_VOLUME], source, dest, CLITERAL(Vector2){0}, 0, ColorBrightness(WHITE, -0.10));

    uint64_t id = DJB2_INIT;
    id = djb2(id, file, strlen(file));
    id = djb2(id, &line, sizeof(line));
    int state = button_with_id(plug, id, boundary);
    bool updated = false;
    if (
        (plug->active_text_input_id == 0 && IsKeyPressed(KEY_TOGGLE_MUTE)) ||
        (state & BS_CLICKED)
    ) {
        if (volume > 0) {
            saved_volume = volume;
            volume = 0;
        } else {
            volume = saved_volume;
        }
        SetMasterVolume(volume);
        updated = true;
        updated = true;
    }

    if (volume <= 0.0) {
        tooltip(plug, boundary, "Unmute [M]", SIDE_TOP, false);
    } else {
        tooltip(plug, boundary, "Mute [M]", SIDE_TOP, false);
    }

    return updated;
}

#define checkbox(plug, value, tooltip_text, boundary) \
    checkbox_with_location((plug), __FILE__, __LINE__, (value), (tooltip_text), (boundary))
static int checkbox_with_location(Plug *plug, const char *file, int line, bool *value, const char *tooltip_text, Rectangle boundary)
{
    uint64_t id = DJB2_INIT;
    id = djb2(id, file, strlen(file));
    id = djb2(id, &line, sizeof(line));

    int state = button_with_id(plug, id, boundary);
    if (state & BS_CLICKED) {
        *value = !*value;
    }

    Rectangle checkbox = {
        .x = boundary.x + boundary.width/2 - boundary.width*0.2f,
        .y = boundary.y + boundary.height/2 - boundary.height*0.2f,
        .width = boundary.width*0.4f,
        .height = boundary.height*0.4f,
    };
    Color color = (state & BS_HOVEROVER) ? COLOR_TRACK_BUTTON_HOVEROVER : COLOR_TRACK_BUTTON_BACKGROUND;
    DrawRectangleRounded(boundary, 0.2f, 20, color);
    if (*value) {
        DrawRectangleRec(checkbox, COLOR_ACCENT);
    }
    DrawRectangleLinesEx(checkbox, 2, WHITE);

    tooltip(plug, boundary, tooltip_text, SIDE_TOP, false);
    return state;
}

#define text_input(plug, text, text_capacity, boundary) \
    text_input_with_location((plug), __FILE__, __LINE__, (text), (text_capacity), (boundary))
static bool text_input_with_location(Plug *plug, const char *file, int line, char *text, size_t text_capacity, Rectangle boundary)
{
    assert(text_capacity > 0);

    uint64_t id = DJB2_INIT;
    id = djb2(id, file, strlen(file));
    id = djb2(id, &line, sizeof(line));

    bool hovered = CheckCollisionPointRec(GetMousePosition(), boundary);
    if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        plug->active_text_input_id = id;
    }

    bool focused = plug->active_text_input_id == id;
    bool changed = false;
    size_t text_length = strlen(text);

    if (focused) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 126 && text_length + 1 < text_capacity) {
                text[text_length++] = (char)key;
                text[text_length] = '\0';
                changed = true;
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && text_length > 0) {
            text[--text_length] = '\0';
            changed = true;
        }
    }

    Color background = focused ? COLOR_TRACK_BUTTON_HOVEROVER : COLOR_TRACK_BUTTON_BACKGROUND;
    Color border = focused ? COLOR_ACCENT : hovered ? COLOR_TRACK_BUTTON_HOVEROVER : COLOR_TRACK_BUTTON_BACKGROUND;
    DrawRectangleRounded(boundary, 0.2f, 20, background);
    DrawRectangleLinesEx(boundary, 2.0f, border);

    float font_size = boundary.height*0.5f;
    float spacing = 0.0f;
    Vector2 text_position = {
        .x = boundary.x + boundary.height*0.3f,
        .y = boundary.y + boundary.height/2 - font_size/2,
    };
    DrawTextEx(plug->font, text, text_position, font_size, spacing, WHITE);

    if (focused && ((int)(GetTime()*2.0) % 2 == 0)) {
        float cursor_x = text_position.x + MeasureTextEx(plug->font, text, font_size, spacing).x;
        DrawLineEx(
            (Vector2) { cursor_x, text_position.y },
            (Vector2) { cursor_x, text_position.y + font_size },
            2.0f,
            WHITE);
    }

    return changed;
}

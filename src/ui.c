static float slider_get_value(float x, float lox, float hix)
{
    if (x < lox) x = lox;
    if (x > hix) x = hix;
    x -= lox;
    x /= hix - lox;
    return x;
}

static bool horz_slider(Plug *plug, Rectangle boundary, float *value, bool *dragging)
{
    bool updated = false;
    Vector2 mouse = GetMousePosition();
    Vector2 startPos = {
        .x = boundary.x + boundary.height/2,
        .y = boundary.y + boundary.height/2,
    };
    Vector2 endPos = {
        .x = boundary.x + boundary.width - boundary.height/2,
        .y = boundary.y + boundary.height/2,
    };
    Color color = WHITE;
    DrawLineEx(startPos, endPos, boundary.height*0.10, color);
    Vector2 center = {
        .x = startPos.x + (endPos.x - startPos.x)*(*value),
        .y = startPos.y,
    };
    float radius = boundary.height/4;
    {
        Texture2D texture = { rlGetTextureIdDefault(), 1, 1, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
        SetShaderValue(plug->circle, plug->circle_radius_location, (float[1]){ 0.43f }, SHADER_UNIFORM_FLOAT);
        SetShaderValue(plug->circle, plug->circle_power_location, (float[1]){ 2.0f }, SHADER_UNIFORM_FLOAT);
        BeginShaderMode(plug->circle);
        Rectangle source = {0, 0, 1, 1};
        Rectangle dest = { center.x - radius, center.y - radius, radius*2, radius*2 };
        Vector2 origin = {0};
        DrawTexturePro(texture, source, dest, origin, 0, color);
        EndShaderMode();
    }

    if (!*dragging) {
        if (CheckCollisionPointCircle(mouse, center, radius)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                *dragging = true;
            }
        } else if (CheckCollisionPointRec(mouse, boundary) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            *value = slider_get_value(mouse.x, startPos.x, endPos.x);
            updated = true;
            *dragging = true;
        }
    } else {
        *value = slider_get_value(mouse.x, startPos.x, endPos.x);
        updated = true;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            *dragging = false;
        }
    }
    return updated;
}

#define volume_slider(plug, boundary) \
    volume_slider_with_location((plug), __FILE__, __LINE__, (boundary))
static bool volume_slider_with_location(Plug *plug, const char *file, int line, Rectangle volume_icon_boundary)
{
    Vector2 mouse = GetMousePosition();
    static int expanded = false;
    static bool dragging = false;
    static float saved_volume = 0.0f;
    Rectangle volume_slider_boundary = volume_icon_boundary;
    size_t expanded_slots = 6;
    if (expanded) volume_slider_boundary.width = expanded_slots*HUD_BUTTON_SIZE;
    expanded = dragging || CheckCollisionPointRec(mouse, volume_slider_boundary);

    float icon_size = 512;
    float scale = HUD_BUTTON_SIZE/icon_size*HUD_ICON_SCALE;
    Rectangle dest = {
        volume_slider_boundary.x + HUD_BUTTON_SIZE/2 - icon_size*scale/2,
        volume_slider_boundary.y + HUD_BUTTON_SIZE/2 - icon_size*scale/2,
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

    bool updated = false;
    if (expanded) {
        Rectangle slider_boundary = {
            .x = volume_slider_boundary.x + HUD_BUTTON_SIZE,
            .y = volume_slider_boundary.y,
            .width = (expanded_slots - 1)*HUD_BUTTON_SIZE,
            .height = HUD_BUTTON_SIZE,
        };
        updated = horz_slider(plug, slider_boundary, &volume, &dragging);
        float mouse_wheel_step = 0.05;
        float wheel_delta = GetMouseWheelMove();
        volume += wheel_delta*mouse_wheel_step;
        if (volume < 0) volume = 0;
        if (volume > 1) volume = 1;
        SetMasterVolume(volume);

        tooltip(plug, slider_boundary, TextFormat("Volume %d%%", (int)floorf(volume*100.0f)), SIDE_TOP, dragging);
    }

    uint64_t id = DJB2_INIT;
    id = djb2(id, file, strlen(file));
    id = djb2(id, &line, sizeof(line));
    int volume_icon_state = button_with_id(plug, id, volume_icon_boundary);
    if (
        (plug->active_text_input_id == 0 && IsKeyPressed(KEY_TOGGLE_MUTE)) ||
        (volume_icon_state & BS_CLICKED)
    ) {
        if (volume > 0) {
            saved_volume = volume;
            volume = 0;
        } else {
            volume = saved_volume;
        }
        SetMasterVolume(volume);
        updated = true;
    }

    if (volume <= 0.0) {
        tooltip(plug, volume_icon_boundary, "Unmute [M]", SIDE_TOP, false);
    } else {
        tooltip(plug, volume_icon_boundary, "Mute [M]", SIDE_TOP, false);
    }

    return dragging || updated;
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
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        plug->active_text_input_id = hovered ? id : 0;
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

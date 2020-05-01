#include "draw_command.h"

/*\
|*| Note that the files aren't actually fully persisted in memory.
|*| Rather, we first load the resource, then check the magic word
|*| and sequence / image size information to verify accuracy, and
|*| finally load the file (while considering the 8 byte offset)
\*/
/* command getting/setting */

n_GDrawCommandType n_gdraw_command_get_type(n_GDrawCommand * command) {
    return command->type;
}

unsigned short n_gdraw_command_get_num_points(n_GDrawCommand * command) {
    return command->num_points;
}

n_GColor n_gdraw_command_get_stroke_color(n_GDrawCommand * command) {
    return command->stroke_color;
}
void n_gdraw_command_set_stroke_color(n_GDrawCommand * command, n_GColor stroke_color) {
    command->stroke_color = stroke_color;
}

char  n_gdraw_command_get_stroke_width(n_GDrawCommand * command) {
    return command->stroke_width;
}
void n_gdraw_command_set_stroke_width(n_GDrawCommand * command, char stroke_width) {
    command->stroke_width = stroke_width;
}

n_GColor n_gdraw_command_get_fill_color(n_GDrawCommand * command) {
    return command->fill_color;
}
void n_gdraw_command_set_fill_color(n_GDrawCommand * command, n_GColor fill_color) {
    command->fill_color = fill_color;
}

n_GPoint n_gdraw_command_get_point(n_GDrawCommand * command, unsigned short index) {
    return command->points[index];
}
void n_gdraw_command_set_point(n_GDrawCommand * command, unsigned short index, n_GPoint point) {
    command->points[index] = point;
}

unsigned short n_gdraw_command_get_radius(n_GDrawCommand * command) {
    return command->circle_radius;
}
void n_gdraw_command_set_radius(n_GDrawCommand * command, unsigned short radius) {
    command->circle_radius = radius;
}

char n_gdraw_command_get_path_open(n_GDrawCommand * command) {
    return command->path_flags.path_open;
}
void n_gdraw_command_set_path_open(n_GDrawCommand * command, char path_open) {
    command->path_flags.path_open = path_open;
}

char n_gdraw_command_get_hidden(n_GDrawCommand * command) {
    return command->flags.hidden;
}
void n_gdraw_command_set_hidden(n_GDrawCommand * command, char hidden) {
    command->flags.hidden = hidden;
}

/* draw: defined for image / frame / sequence */

void n_gdraw_command_draw(n_GContext * ctx, n_GDrawCommand * command, n_GPoint offset) {
#ifdef PBL_BW
    static const char bw_lookup[] = {0b00000000, 0b11101010, 0b11000000, 0b11111111};
    if (command->flags.use_bw_color) {
        n_graphics_context_set_stroke_color(ctx, (n_GColor) { .argb = bw_lookup[command->flags.bw_stroke & 0b11] });
        n_graphics_context_set_fill_color(ctx, (n_GColor) { .argb = bw_lookup[command->flags.bw_fill & 0b11] });
    } else {
#endif
        n_graphics_context_set_stroke_color(ctx, command->stroke_color);
        n_graphics_context_set_fill_color(ctx, command->fill_color);
#ifdef PBL_BW
    }
#endif
    n_graphics_context_set_stroke_width(ctx, command->stroke_width);
    // Note that fill_path and draw_path (and their ppath equivalents)
    // are private apis. Therefore, they currently ignore the alpha component.
    n_GPoint * pts_transformed;
    switch (command->type) {
        case n_GDrawCommandTypePath:
            pts_transformed = NGFX_PREFERRED_malloc(sizeof(n_GPoint) * command->num_points);
            if (!pts_transformed) { break; }
            n_prv_transform_points(command->num_points, command->points,
                pts_transformed, 0, offset);
            if (ctx->fill_color.argb & (0b11 << 6))
                n_graphics_fill_path(ctx, command->num_points, pts_transformed);
            if (ctx->stroke_color.argb & (0b11 << 6))
                n_graphics_draw_path(ctx, command->num_points, pts_transformed, command->path_flags.path_open);
            NGFX_PREFERRED_free(pts_transformed);
            break;
        case n_GDrawCommandTypeCircle:
            for (unsigned int i = 0; i < command->num_points; i++) {
                n_graphics_fill_circle(ctx, n_GPoint(
                        command->points[i].x + offset.x,
                        command->points[i].y + offset.y),
                    command->circle_radius);
                n_graphics_draw_circle(ctx, n_GPoint(
                        command->points[i].x + offset.x,
                        command->points[i].y + offset.y),
                    command->circle_radius);
            }
            break;
        case n_GDrawCommandTypePrecisePath:
            pts_transformed = NGFX_PREFERRED_malloc(sizeof(n_GPoint) * command->num_points);
            if (!pts_transformed) { break; }
            n_prv_transform_points(command->num_points, command->points,
                pts_transformed, 0, n_GPoint(offset.x << 3, offset.y << 3));
            if (ctx->fill_color.argb & (0b11 << 6))
                n_graphics_fill_ppath(ctx, command->num_points, pts_transformed);
            if (ctx->stroke_color.argb & (0b11 << 6))
                n_graphics_draw_ppath(ctx, command->num_points, pts_transformed, command->path_flags.path_open);
            NGFX_PREFERRED_free(pts_transformed);
            break;
        case n_GDrawCommandTypePreciseCircle:
            for (unsigned int i = 0; i < command->num_points; i++) {
                n_graphics_fill_circle(ctx, n_GPoint(
                        ((command->points[i].x + 4) >> 3) + offset.x,
                        ((command->points[i].y + 4) >> 3) + offset.y),
                    command->circle_radius);
                n_graphics_draw_circle(ctx, n_GPoint(
                        ((command->points[i].x + 4) >> 3) + offset.x,
                        ((command->points[i].y + 4) >> 3) + offset.y),
                    command->circle_radius);
            }
            break;
        default:
            break;
    }
}

void n_gdraw_command_image_draw(n_GContext * ctx, n_GDrawCommandImage * image, n_GPoint offset) {
    n_gdraw_command_list_draw(ctx, n_gdraw_command_image_get_command_list(image), offset);
}

void n_gdraw_command_frame_draw(n_GContext * ctx, n_GDrawCommandSequence * sequence, n_GDrawCommandFrame * frame, n_GPoint offset) {
    n_gdraw_command_list_draw(ctx, n_gdraw_command_frame_get_command_list(frame), offset);
}

typedef struct {
    n_GContext * ctx; n_GPoint offset;
} nPrvGDrawCommandListDrawContext;

static char n_prv_gdraw_command_draw_cb(n_GDrawCommand * command, unsigned int index, void * context) {
    n_gdraw_command_draw(((nPrvGDrawCommandListDrawContext *) context)->ctx,
                         command,
                         ((nPrvGDrawCommandListDrawContext *) context)->offset);
    return true;
}

void n_gdraw_command_list_draw(n_GContext * ctx, n_GDrawCommandList * list, n_GPoint offset) {
    nPrvGDrawCommandListDrawContext * context =
        NGFX_PREFERRED_malloc(sizeof(nPrvGDrawCommandListDrawContext));
    context->ctx = ctx; context->offset = offset;
    n_gdraw_command_list_iterate(list, n_prv_gdraw_command_draw_cb, context);
    NGFX_PREFERRED_free(context);
}

/* command list getters */

n_GDrawCommandList * n_gdraw_command_image_get_command_list(n_GDrawCommandImage * image) {
    return image->command_list;
}
n_GDrawCommandList * n_gdraw_command_frame_get_command_list(n_GDrawCommandFrame * frame) {
    return frame->command_list;
}

unsigned int n_gdraw_command_list_get_num_commands(n_GDrawCommandList * list) {
    return list->num_commands;
}
n_GDrawCommand * n_gdraw_command_list_get_command(n_GDrawCommandList * list, unsigned int index) {
    n_GDrawCommand * cmd = list->commands;
    for (unsigned int i = 0; i < index; i++) {
        cmd = (n_GDrawCommand *) (cmd->points + cmd->num_points);
    }
    return cmd;
}

/* miscellaneous list-only */

void n_gdraw_command_list_iterate(n_GDrawCommandList * list, n_GDrawCommandListIteratorCb cb, void * cb_context) {
    n_GDrawCommand * cmd = list->commands;
    for (unsigned int i = 0; i < list->num_commands; i++) {
        if (!cb(cmd, i, cb_context))
            return;
        cmd = (n_GDrawCommand *) (cmd->points + cmd->num_points);
    }
}


/* miscellaneous frame-only */

unsigned short n_gdraw_command_frame_get_duration(n_GDrawCommandFrame * frame) {
    return frame->duration; }
void n_gdraw_command_frame_set_duration(n_GDrawCommandFrame * frame, unsigned short duration) {
    frame->duration = duration; }

/* miscellaneous sequence-only */

n_GDrawCommandFrame * n_gdraw_command_sequence_get_frame_by_elapsed(n_GDrawCommandSequence * sequence, unsigned int ms) {
    n_GDrawCommandFrame * frame = sequence->frames, * last_frame = NULL;
    unsigned short index = 0;
    for (unsigned int elapsed = 0; elapsed <= ms;) {
        if (++index == sequence->num_frames)
            return frame;
        last_frame = frame;
        elapsed += frame->duration;
            // doing the following works because by using the number of commands,
            // we inherently get the next frame instead.
        frame = (n_GDrawCommandFrame *) n_gdraw_command_list_get_command(frame->command_list, frame->command_list->num_commands);
    }
    return last_frame;
}
n_GDrawCommandFrame * n_gdraw_command_sequence_get_frame_by_index(n_GDrawCommandSequence * sequence, unsigned int index) {
    return NULL; }
unsigned int n_gdraw_command_sequence_get_play_count(n_GDrawCommandSequence * sequence) {
    return sequence->play_count; }
void n_gdraw_command_sequence_set_play_count(n_GDrawCommandSequence * sequence, unsigned short play_count) {
    sequence->play_count = play_count; }

unsigned int n_gdraw_command_sequence_get_total_duration(n_GDrawCommandSequence * sequence) {
    n_GDrawCommandFrame * frame = sequence->frames;
    unsigned int duration = 0;
    for (unsigned short index = 0; index < sequence->num_frames; index += 1) {
        duration += frame->duration;
        frame = (n_GDrawCommandFrame *) n_gdraw_command_list_get_command(frame->command_list, frame->command_list->num_commands);
    }
    return duration; }

unsigned short n_gdraw_command_sequence_get_num_frames(n_GDrawCommandSequence * sequence) {
    return sequence->num_frames; }

/* get / set bounds: defined for image and sequence */

n_GSize n_gdraw_command_image_get_bounds_size(n_GDrawCommandImage * image) {
    return image->view_box; }
void n_gdraw_command_image_set_bounds_size(n_GDrawCommandImage * image, n_GSize size) {
    image->view_box = size; }

n_GSize n_gdraw_command_sequence_get_bounds_size(n_GDrawCommandSequence * sequence) {
    return sequence->view_box; }
void n_gdraw_command_sequence_set_bounds_size(n_GDrawCommandSequence * sequence, n_GSize size) {
    sequence->view_box = size; }

/* create with resource / clone / destroy */

n_GDrawCommandImage * n_gdraw_command_image_create_with_resource(unsigned int resource_id) {
    ResHandle handle = resource_get_handle(resource_id);
    size_t image_size = resource_size(handle);
    n_GDrawCommandImage * image = NGFX_PREFERRED_malloc(image_size);
    if (image) {
        NGFX_PREFERRED_resource_load(handle, (char *) image, image_size);
        return image;
    }
    return NULL;
}
n_GDrawCommandImage * n_gdraw_command_image_clone(n_GDrawCommandImage * image) {
    return NULL; } // TODO
void n_gdraw_command_image_destroy(n_GDrawCommandImage * image) {
    NGFX_PREFERRED_free(image);
}

n_GDrawCommandSequence * n_gdraw_command_sequence_create_with_resource(unsigned int resource_id) {
    ResHandle handle = resource_get_handle(resource_id);
    size_t sequence_size = resource_size(handle);
    n_GDrawCommandSequence * sequence = NGFX_PREFERRED_malloc(sequence_size);
    if (sequence) {
        NGFX_PREFERRED_resource_load(handle, (char *) sequence, sequence_size);
        return sequence;
    }
    return NULL;;
}
n_GDrawCommandSequence * n_gdraw_command_sequence_clone(n_GDrawCommandSequence * image) {
    return NULL; }
void n_gdraw_command_sequence_destroy(n_GDrawCommandSequence * sequence) {
    NGFX_PREFERRED_free(sequence);
}

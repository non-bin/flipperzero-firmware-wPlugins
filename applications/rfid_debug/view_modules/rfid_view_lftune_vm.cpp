#include "rfid_view_lftune_vm.h"
#include <callback-connector.h>
#include <gui/elements.h>

struct RfidViewLFTuneVMModel {
    bool dirty;
    bool fine;
    uint32_t ARR;
    uint32_t CCR;
    int pos;
};

void RfidViewLFTuneVM::view_draw_callback(Canvas* canvas, void* _model) {
    RfidViewLFTuneVMModel* model = reinterpret_cast<RfidViewLFTuneVMModel*>(_model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    if(model->fine) {
        canvas_draw_box(
            canvas,
            128 - canvas_string_width(canvas, "Fine") - 4,
            0,
            canvas_string_width(canvas, "Fine") + 4,
            canvas_current_font_height(canvas) + 1);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_str_aligned(canvas, 128 - 2, 2, AlignRight, AlignTop, "Fine");
    canvas_set_color(canvas, ColorBlack);

    constexpr uint8_t buffer_size = 128;
    char buffer[buffer_size + 1];
    double freq = ((float)SystemCoreClock / ((float)model->ARR + 1));
    double duty = ((float)model->CCR + 1) / ((float)model->ARR + 1) * 100.0f;
    snprintf(
        buffer,
        buffer_size,
        "%sARR: %lu\n"
        "freq = %.4f\n"
        "%sCCR: %lu\n"
        "duty = %.4f",
        model->pos == 0 ? ">" : "",
        model->ARR,
        freq,
        model->pos == 1 ? ">" : "",
        model->CCR,
        duty);
    elements_multiline_text_aligned(canvas, 2, 2, AlignLeft, AlignTop, buffer);
}

bool RfidViewLFTuneVM::view_input_callback(InputEvent* event, void* context) {
    RfidViewLFTuneVM* _this = reinterpret_cast<RfidViewLFTuneVM*>(context);
    bool consumed = false;

    // Process key presses only
    if(event->type == InputTypeShort || event->type == InputTypeRepeat) {
        consumed = true;

        switch(event->key) {
        case InputKeyLeft:
            _this->button_left();
            break;
        case InputKeyRight:
            _this->button_right();
            break;
        case InputKeyUp:
            _this->button_up();
            break;
        case InputKeyDown:
            _this->button_down();
            break;
        case InputKeyOk:
            _this->button_ok();
            break;
        default:
            consumed = false;
            break;
        }
    }

    return consumed;
}

void RfidViewLFTuneVM::button_up() {
    with_view_model_cpp(view, RfidViewLFTuneVMModel, model, {
        if(model->pos > 0) model->pos--;
        return true;
    });
}

void RfidViewLFTuneVM::button_down() {
    with_view_model_cpp(view, RfidViewLFTuneVMModel, model, {
        if(model->pos < 1) model->pos++;
        return true;
    });
}

void RfidViewLFTuneVM::button_left() {
    with_view_model_cpp(view, RfidViewLFTuneVMModel, model, {
        if(model->pos == 0) {
            if(model->fine) {
                model->ARR -= 1;
            } else {
                model->ARR -= 10;
            }
        } else if(model->pos == 1) {
            if(model->fine) {
                model->CCR -= 1;
            } else {
                model->CCR -= 10;
            }
        }

        model->dirty = true;
        return true;
    });
}

void RfidViewLFTuneVM::button_right() {
    with_view_model_cpp(view, RfidViewLFTuneVMModel, model, {
        if(model->pos == 0) {
            if(model->fine) {
                model->ARR += 1;
            } else {
                model->ARR += 10;
            }
        } else if(model->pos == 1) {
            if(model->fine) {
                model->CCR += 1;
            } else {
                model->CCR += 10;
            }
        }

        model->dirty = true;
        return true;
    });
}

void RfidViewLFTuneVM::button_ok() {
    with_view_model_cpp(view, RfidViewLFTuneVMModel, model, {
        model->fine = !model->fine;
        return true;
    });
}

RfidViewLFTuneVM::RfidViewLFTuneVM() {
    view = view_alloc();
    view_set_context(view, this);
    view_allocate_model(view, ViewModelTypeLocking, sizeof(RfidViewLFTuneVMModel));

    with_view_model_cpp(view, RfidViewLFTuneVMModel, model, {
        model->dirty = true;
        model->fine = false;
        model->ARR = 511;
        model->CCR = 255;
        model->pos = 0;
        return true;
    });

    view_set_draw_callback(
        view, cbc::obtain_connector(this, &RfidViewLFTuneVM::view_draw_callback));
    view_set_input_callback(
        view, cbc::obtain_connector(this, &RfidViewLFTuneVM::view_input_callback));
}

RfidViewLFTuneVM::~RfidViewLFTuneVM() {
    view_free(view);
}

View* RfidViewLFTuneVM::get_view() {
    return view;
}

void RfidViewLFTuneVM::clean() {
    with_view_model_cpp(view, RfidViewLFTuneVMModel, model, {
        model->dirty = true;
        model->fine = false;
        model->ARR = 511;
        model->CCR = 255;
        model->pos = 0;
        return true;
    });
}

bool RfidViewLFTuneVM::is_dirty() {
    bool result;
    with_view_model_cpp(view, RfidViewLFTuneVMModel, model, {
        result = model->dirty;
        model->dirty = false;
        return false;
    });

    return result;
}

uint32_t RfidViewLFTuneVM::get_ARR() {
    uint32_t result;
    with_view_model_cpp(view, RfidViewLFTuneVMModel, model, {
        result = model->ARR;
        return false;
    });

    return result;
}

uint32_t RfidViewLFTuneVM::get_CCR() {
    uint32_t result;
    with_view_model_cpp(view, RfidViewLFTuneVMModel, model, {
        result = model->CCR;
        return false;
    });

    return result;
}

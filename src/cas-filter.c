#include <obs-module.h>

struct cas_filter_data {
    obs_source_t *context;

    bool enabled;

    int roi_left;
    int roi_bottom;
    int roi_right;
    int roi_top;

    int per_pixel_err_thresh;
    int total_match_thresh;

    bool frame_wanted;
    bool frame_available;
    gs_texrender_t *frame_copy;
    bool frame_matched_prev;
    bool frame_matched;
};

static const char *cas_filter_get_name(void* unused)
{
        UNUSED_PARAMETER(unused);
        return obs_module_text("ContentActivatedSwitcher");
}

static void *cas_filter_create(obs_data_t *settings, obs_source_t *context)
{
        struct cas_filter_data *filter = bzalloc(sizeof(*filter));
        // char *effect_path = obs_module_file("cas_filter.effect");

        filter->context = context;

        //obs_enter_graphics();
        //filter->effect = gs_effect_create_from_file(effect_path, NULL);
        //obs_leave_graphics();
        // bfree(effect_path)
        // if (!filter->effect) {
        //         bfree(filter);
        //         return NULL;
        // }

        obs_source_update(context, settings);
        return filter;
}

static void cas_filter_destroy(void *data)
{
        struct cas_filter_data *filter = data;

        //obs_enter_graphics();
        //gs_effect_destroy(filter->effect);
        //obs_leave_graphics();

        bfree(filter);
}

static void cas_filter_update(void *data, obs_data_t *settings)
{
        struct cas_filter_data *filter = data;

        filter->enabled = obs_data_get_bool(settings, "enabled");
        filter->roi_left = (int)obs_data_get_int(settings, "roi_left");
        filter->roi_bottom = (int)obs_data_get_int(settings, "roi_bottom");
        filter->roi_right = (int)obs_data_get_int(settings, "roi_right");
        filter->roi_top = (int)obs_data_get_int(settings, "roi_top");
        filter->per_pixel_err_thresh
                = (int)obs_data_get_int(settings, "per_pixel_err_thresh");
        filter->total_match_thresh
                = (int)obs_data_get_int(settings, "total_match_thresh");
        // TODO mask image
        // TODO scenes

        UNUSED_PARAMETER(data);
}

static bool cas_prop_changed_callback(
        obs_properties_t *props, obs_property_t *p, obs_data_t *settings)
{
        UNUSED_PARAMETER(props);
        UNUSED_PARAMETER(p);
        UNUSED_PARAMETER(settings);
}

static obs_properties_t *cas_filter_properties(void *data)
{
        obs_properties_t *props = obs_properties_create();

        obs_property_t *p = obs_properties_add_bool(
                props, "enabled", obs_module_text("Enabled"));

        obs_properties_add_int(props,
                "roi_left", obs_module_text("RoiLeft"),
                0, 10000, 1);
        obs_properties_add_int(props,
                "roi_bottom", obs_module_text("RoiBottom"),
                0, 10000, 1);
        obs_properties_add_int(props,
                "roi_right", obs_module_text("RoiRight"),
                0, 10000, 1);
        obs_properties_add_int(props,
                "roi_top", obs_module_text("RoiTop"),
                0, 10000, 1);

        obs_properties_add_int(props,
                "per_pixel_err_thresh",
                obs_module_text("PerPixelErrorThreshold"),
                0, 100, 1);
        obs_properties_add_int(props,
                "total_match_thresh",
                obs_module_text("TotalMatchThreshold"),
                0, 100, 1);

        obs_property_set_modified_callback(p, cas_prop_changed_callback);

        // TODO scenes
        // TODO mask image

        UNUSED_PARAMETER(data);
        return props;
}

static void cas_filter_defaults(obs_data_t *settings)
{
        obs_data_set_default_bool(settings, "enabled", false);
        obs_data_set_default_int(settings, "per_pixel_err_thresh", 10);
        obs_data_set_default_int(settings, "total_match_thresh", 90);
}

static void cas_filter_tick(void *data, float seconds)
{
        struct cas_filter_data *filter = data;
        UNUSED_PARAMETER(seconds);
        UNUSED_PARAMETER(filter);
        // TODO: dispatch pixel processing every 100ms?
}

static void cas_filter_render(void *data, gs_effect_t *effect)
{
        struct cas_filter_data *filter = data;

        // TODO: render data to an RGB tex, when it's time
        obs_source_skip_video_filter(filter->context);
        UNUSED_PARAMETER(effect);
        return;
}

static uint32_t cas_filter_width(void *data)
{
        struct cas_filter_data *filter = data;
        obs_source_t *parent = obs_filter_get_parent(filter->context);
        return obs_source_get_base_width(parent);
}

static uint32_t cas_filter_height(void *data)
{
        struct cas_filter_data *filter = data;
        obs_source_t *parent = obs_filter_get_parent(filter->context);
        return obs_source_get_base_height(parent);
}

struct obs_source_info cas_filter = {
        .id = "cas_filter",
        .type = OBS_SOURCE_TYPE_FILTER,
        .output_flags = OBS_SOURCE_VIDEO,
        .get_name = cas_filter_get_name,
        .create = cas_filter_create,
        .destroy = cas_filter_destroy,
        .update = cas_filter_update,
        .get_properties = cas_filter_properties,
        .get_defaults = cas_filter_defaults,
        .video_tick = cas_filter_tick,
        .video_render = cas_filter_render,
        .get_width = cas_filter_width,
        .get_height = cas_filter_height,
};

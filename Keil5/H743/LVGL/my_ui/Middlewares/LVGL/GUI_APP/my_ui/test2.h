#ifndef __test2_H__
#define __test2_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Act_manager.h"

    void test2_test();
    app_def_t* test2_def_get();

    lv_obj_t* test2_create_cb(void);
    lv_obj_t* test2_destroy_cb(struct activity_t* activity);
    lv_obj_t* test2_pause_cb(struct activity_t* activity);
    lv_obj_t* test2_resume_cb(struct activity_t* activity);

#ifdef __cplusplus
}
#endif

#endif

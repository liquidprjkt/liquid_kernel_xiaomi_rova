/*
 *
 * FocalTech TouchScreen driver.
 *
 * Copyright (c) 2010-2017, FocalTech Systems, Ltd., all rights reserved.
 * Copyright (C) 2018 XiaoMi, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "focaltech_core.h"

#if FTS_POINT_REPORT_CHECK_EN
#define POINT_REPORT_CHECK_WAIT_TIME              200
static struct delayed_work fts_point_report_check_work;
static struct workqueue_struct *fts_point_report_check_workqueue = NULL;

static void fts_point_report_check_func(struct work_struct *work)
{

#if FTS_MT_PROTOCOL_B_EN
	unsigned int finger_count=0;
#endif

	FTS_FUNC_ENTER();
	mutex_lock(&ft5435_fts_wq_data->report_mutex);

#if FTS_MT_PROTOCOL_B_EN
	for (finger_count = 0; finger_count < ft5435_fts_wq_data->pdata->max_touch_number; finger_count++) {
		 input_mt_slot(ft5435_fts_input_dev, finger_count);
		 input_mt_report_slot_state(ft5435_fts_input_dev, MT_TOOL_FINGER, false);
	}
#else
	input_mt_sync(ft5435_fts_input_dev);
#endif
	input_report_key(ft5435_fts_input_dev, BTN_TOUCH, 0);
	input_sync(ft5435_fts_input_dev);

	mutex_unlock(&ft5435_fts_wq_data->report_mutex);

	FTS_FUNC_EXIT();
}

void fts_point_report_check_queue_work(void)
{
	cancel_delayed_work(&fts_point_report_check_work);
	queue_delayed_work(fts_point_report_check_workqueue, &fts_point_report_check_work, msecs_to_jiffies(POINT_REPORT_CHECK_WAIT_TIME));
}

int fts_point_report_check_init(void)
{
	FTS_FUNC_ENTER();

	INIT_DELAYED_WORK(&fts_point_report_check_work, fts_point_report_check_func);
	fts_point_report_check_workqueue = create_workqueue("fts_point_report_check_func_wq");
	if (fts_point_report_check_workqueue == NULL) {
		 FTS_ERROR("[POINT_REPORT]: Failed to create fts_point_report_check_workqueue!!");
	} else {
		 FTS_DEBUG("[POINT_REPORT]: Success to create fts_point_report_check_workqueue!!");
	}

	FTS_FUNC_EXIT();

	return 0;
}

int fts_point_report_check_exit(void)
{
	FTS_FUNC_ENTER();

	destroy_workqueue(fts_point_report_check_workqueue);

	FTS_FUNC_EXIT();
	return 0;
}
#endif


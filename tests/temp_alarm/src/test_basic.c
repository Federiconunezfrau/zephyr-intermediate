/*
 * temp_alarm - Happy-path unit tests (homework skeleton)
 *
 * test_status_after_init is provided as a worked example. Finish Task 0 first,
 * then fill in the Task 1 stubs according to TEST_SPEC.md. The stubs call
 * ztest_test_skip(), so the binary builds and runs cleanly until each test is
 * implemented.
 *
 * Run:
 *   west twister -T tests/temp_alarm -p native_sim
 */

#include <errno.h>
#include <zephyr/fff.h>
#include <zephyr/ztest.h>

#include "sensor_fake.h"
#include "temp_alarm.h"

#define DEFAULT_THRESHOLD 30

/*
 * Shared by every suite: a clean fake, then the module initialised
 * with the fake device. temp_alarm_init() resets all module state,
 * so no deinit is needed between tests.
 */
static void before(void *fixture)
{
	ARG_UNUSED(fixture);

	sensor_fake_reset();
	sensor_fake_init();

	zassume_ok(temp_alarm_init(&fake_sensor_dev, DEFAULT_THRESHOLD),
		   "precondition: temp_alarm_init must succeed");
}

/*
 * ============================================================================
 * Test Suite: temp_alarm_get_status
 *
 * The status right after a successful init.
 * ============================================================================
 */
ZTEST_SUITE(temp_alarm_get_status, NULL, NULL, before, NULL, NULL);

/* PROVIDED — study this test before writing the rest. */
ZTEST(temp_alarm_get_status, test_status_after_init)
{
	struct temp_alarm_status status;

	zassert_ok(temp_alarm_get_status(&status), "get_status must succeed");

	zassert_equal(status.last_temp, 0, "last_temp must be 0 after init, got %d",
		      status.last_temp);
	zassert_equal(status.alarm_count, 0U, "alarm_count must be 0 after init, got %u",
		      status.alarm_count);
	zassert_false(status.is_alarming, "is_alarming must be false after init");
	zassert_equal(status.last_error, 0, "last_error must be 0 after init, got %d",
		      status.last_error);
}

/*
 * ============================================================================
 * Test Suite: temp_alarm_check
 *
 * Readings below, at and above the threshold.
 * ============================================================================
 */
ZTEST_SUITE(temp_alarm_check, NULL, NULL, before, NULL, NULL);

ZTEST(temp_alarm_check, test_below_threshold)
{
	/* TODO(l8-task1): a reading of 20 C is below the threshold.
	 * Check the status, and how the module talked to the sensor.
	 * See TEST_SPEC.md "Suite temp_alarm_check" #1.
	 */

	int ret;
	struct temp_alarm_status status;

	// The precondition of 20° is done here because the before hook is shared
	// among every test
	sensor_fake_set_temperature(20);

	ret = temp_alarm_check();
	zassert_ok(ret, "Check should succeed, but got error %d", ret);
	ret = temp_alarm_get_status(&status);
	zassert_ok(ret, "get_status should succeed");
	zassert_equal(status.last_temp, 20, "last_temp must be 20, got %d", status.last_temp);
	zassert_false(status.is_alarming, "is_alarming must be false");
	zassert_equal(fake_sensor_sample_fetch_fake.call_count, 1,
	            "The sensor should have sampled only once the temperature, but did it %d times", fake_sensor_sample_fetch_fake.call_count);
	zassert_equal(fake_sensor_channel_get_fake.arg1_val, SENSOR_CHAN_AMBIENT_TEMP,
	            "The sample fetched should be from channel SENSOR_CHAN_AMBIENT_TEMP, but was from %u", fake_sensor_channel_get_fake.arg1_val);
}

ZTEST(temp_alarm_check, test_at_threshold)
{
	/* TODO(l8-task1): a reading equal to the threshold (30 C) is not an alarm.
	 * See TEST_SPEC.md "Suite temp_alarm_check" #2.
	 */
	int ret;
	struct temp_alarm_status status;

	// The precondition of 30° is done here because the before hook is shared
	// among every test
	sensor_fake_set_temperature(30);

	ret = temp_alarm_check();
	zassert_ok(ret, "Check should succeed, but got error %d", ret);
	ret = temp_alarm_get_status(&status);
	zassert_ok(ret, "get_status should succeed");
	zassert_false(status.is_alarming, "is_alarming must be false");
	zassert_equal(status.alarm_count, 0U, "alarm_count must be 0, got %u", status.alarm_count);
	zassert_false(fake_sensor_channel_get_fake.return_val > DEFAULT_THRESHOLD,
	            "The temperature should not be above the threshold %d, but got %d", DEFAULT_THRESHOLD, fake_sensor_channel_get_fake.return_val);
}

ZTEST(temp_alarm_check, test_above_threshold)
{
	/* TODO(l8-task1): a reading of 40 C raises the alarm and counts it.
	 * See TEST_SPEC.md "Suite temp_alarm_check" #3.
	 */
	int ret;
	struct temp_alarm_status status;

	// The precondition of 40° is done here because the before hook is shared
	// among every test
	sensor_fake_set_temperature(40);

	ret = temp_alarm_check();
	zassert_ok(ret, "Check should succeed, but got error %d", ret);
	ret = temp_alarm_get_status(&status);
	zassert_ok(ret, "get_status should succeed");
	zassert_equal(status.last_temp, 40, "last_temp must be 40, got %d", status.last_temp);
	zassert_true(status.is_alarming, "is_alarming must be true");
	zassert_equal(status.alarm_count, 1U, "alarm_count must be 1, got %u", status.alarm_count);
}

/*
 * ============================================================================
 * Test Suite: temp_alarm_threshold
 *
 * Changing the threshold at run time.
 * ============================================================================
 */
ZTEST_SUITE(temp_alarm_threshold, NULL, NULL, before, NULL, NULL);

ZTEST(temp_alarm_threshold, test_new_threshold_clears_alarm)
{
	/* TODO(l8-task1): raise the alarm, then move the threshold
	 * above the reading. The alarm must clear and stay cleared.
	 * See TEST_SPEC.md "Suite temp_alarm_threshold" #1.
	 */
	int ret;
	struct temp_alarm_status status;

	// The precondition of 40° is done here because the before hook is shared
	// among every test
	sensor_fake_set_temperature(40);

	// 40° should trigger the alarm
	ret = temp_alarm_check();
	zassert_ok(ret, "Check should succeed, but got error %d", ret);
	ret = temp_alarm_get_status(&status);
	zassert_ok(ret, "get_status should succeed");
	zassert_true(status.is_alarming, "is_alarming must be true");

	// Sets the new threshold of 50°, does not check again but the alarm should
	// be cleared
	ret = temp_alarm_set_threshold(50);
	zassert_ok(ret, "Setting a new threshold should succeed, but got error %d", ret);
	ret = temp_alarm_get_status(&status);
	zassert_ok(ret, "get_status should succeed");
	zassert_false(status.is_alarming, "is_alarming must be false");

	// Checks the new temperature, the alarm should be clear too but the alarm count
	// should be 1, because it was already triggered before changing the threshold
	ret = temp_alarm_check();
	zassert_ok(ret, "Check should succeed, but got error %d", ret);
	ret = temp_alarm_get_status(&status);
	zassert_false(status.is_alarming, "is_alarming must be false");
	zassert_equal(status.alarm_count, 1U, "alarm_count must be 1, got %u", status.alarm_count);
}

/*
 * ============================================================================
 * Test Suite: temp_alarm_reset
 *
 * temp_alarm_reset() clears the alarm but keeps the last reading.
 * ============================================================================
 */
ZTEST_SUITE(temp_alarm_reset, NULL, NULL, before, NULL, NULL);

ZTEST(temp_alarm_reset, test_reset_clears_state)
{
	/* TODO(l8-task1): raise the alarm three times, then reset.
	 * The alarm state must clear, but the last reading must stay.
	 * See TEST_SPEC.md "Suite temp_alarm_reset" #1.
	 */
	int ret;
	struct temp_alarm_status status;

	// The precondition of 40° is done here because the before hook is shared
	// among every test
	sensor_fake_set_temperature(40);

	// Checks the alarm 3 times, the alarm count should go to 3
	for(uint8_t i = 0; i < 3; i++) {
	    ret = temp_alarm_check();
	    zassert_ok(ret, "Check should succeed, but got error %d", ret);
	}
	ret = temp_alarm_get_status(&status);
	zassert_ok(ret, "get_status should succeed");
	zassert_equal(status.alarm_count, 3U, "alarm_count must be 3, got %u", status.alarm_count);

	// Resets the alarm and gets the new status
	temp_alarm_reset();
	ret = temp_alarm_get_status(&status);
	zassert_ok(ret, "get_status should succeed");
	zassert_false(status.is_alarming, "is_alarming must be false");
	zassert_equal(status.alarm_count, 0U, "After the reset, alarm_count must be 0, got %u", status.alarm_count);
	zassert_ok(status.last_error, "After the reset last_error should be 0, got %d", status.last_error);
	zassert_equal(status.last_temp, 40, "After the reset last_temp should still be be 40, got %d", status.last_temp);
}

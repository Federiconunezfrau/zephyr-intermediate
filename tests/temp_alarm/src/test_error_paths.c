/*
 * temp_alarm - Error-path unit tests (homework skeleton)
 *
 * Task 2: fill in the stubs according to TEST_SPEC.md. Each stub calls
 * ztest_test_skip(), so the binary builds and runs cleanly until the test is
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

/* Same before() hook as in test_basic.c: a clean fake and an initialised module. */
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
 * Test Suite: temp_alarm_sensor_errors
 *
 * Failures from the sensor driver, injected through the fakes.
 * ============================================================================
 */
ZTEST_SUITE(temp_alarm_sensor_errors, NULL, NULL, before, NULL, NULL);

ZTEST(temp_alarm_sensor_errors, test_fetch_error_propagates)
{
	/* TODO(l8-task2): a failed fetch is reported,
	 * and the module does not go on to read the channel.
	 * See TEST_SPEC.md "Suite temp_alarm_sensor_errors" #1.
	 */
	int ret;
	struct temp_alarm_status status;

	// The precondition is done here because the before hook is shared
	// among every test
	fake_sensor_sample_fetch_fake.return_val = -EIO;

	ret = temp_alarm_check();
	zassert_not_ok(ret, "Check should fail");
	zassert_equal(ret, -EIO, "Check should fail with error -EIO, got %d", ret);
	ret = temp_alarm_get_status(&status);
	zassert_ok(ret, "get_status should succeed");
	zassert_equal(status.last_error, -EIO, "last_error should be -EIO, got %d", status.last_error);
	zassert_equal(fake_sensor_channel_get_fake.call_count, 0,
	            "channel_get should not have been called because sample_fetch failed, but it was called %d times", fake_sensor_channel_get_fake.call_count);
}

ZTEST(temp_alarm_sensor_errors, test_channel_get_error_propagates)
{
	/* TODO(l8-task2): a failed channel read is reported.
	 * See TEST_SPEC.md "Suite temp_alarm_sensor_errors" #2.
	 */
	int ret;
	struct temp_alarm_status status;

	// The precondition is done here because the before hook is shared
	// among every test
	fake_sensor_channel_get_fake.return_val = -EIO;

	// The custom_fake is cleared because of precedence
	fake_sensor_channel_get_fake.custom_fake = NULL;

	ret = temp_alarm_check();
	zassert_not_ok(ret, "Check should fail");
	zassert_equal(ret, -EIO, "Check should fail with error -EIO, got %d", ret);
	ret = temp_alarm_get_status(&status);
	zassert_ok(ret, "get_status should succeed");
	zassert_equal(status.last_error, -EIO, "last_error should be -EIO, got %d", status.last_error);
	zassert_equal(fake_sensor_sample_fetch_fake.call_count, 1,
	            "sample_fetch should have been called once, but it was called %d times", fake_sensor_sample_fetch_fake.call_count);
}

ZTEST(temp_alarm_sensor_errors, test_fail_then_recover)
{
	/* TODO(l8-task2): the first fetch fails and the second one succeeds.
	 * The module must recover on the second check.
	 * See TEST_SPEC.md "Suite temp_alarm_sensor_errors" #3.
	 */
	int ret;
	struct temp_alarm_status status;
	int returnValSequence[] = {-EIO, 0};

	// The precondition is done here because the before hook is shared
	// among every test
	SET_RETURN_SEQ(fake_sensor_sample_fetch, returnValSequence, ARRAY_SIZE(returnValSequence));
	sensor_fake_set_temperature(20);

	// First check should fail
	ret = temp_alarm_check();
	zassert_not_ok(ret, "Check should fail");
	zassert_equal(ret, -EIO, "Check should fail with error -EIO, got %d", ret);

	// Second check should succeed
	ret = temp_alarm_check();
	zassert_ok(ret, "Check should succeed");

	ret = temp_alarm_get_status(&status);
	zassert_ok(ret, "get_status should succeed");
	zassert_equal(status.last_temp, 20, "last_temp must be 20, got %d", status.last_temp);
	zassert_false(status.is_alarming, "is_alarming must be false");
	zassert_equal(status.last_error, -EIO, "last_error should be -EIO, got %d", status.last_error);
}

/*
 * ============================================================================
 * Test Suite: temp_alarm_invalid_input
 *
 * Arguments the public API must reject.
 * ============================================================================
 */
ZTEST_SUITE(temp_alarm_invalid_input, NULL, NULL, before, NULL, NULL);

ZTEST(temp_alarm_invalid_input, test_get_status_null)
{
	/* TODO(l8-task2): a NULL status pointer is rejected.
	 * See TEST_SPEC.md "Suite temp_alarm_invalid_input" #1.
	 */
	int ret;

	ret = temp_alarm_get_status(NULL);
	zassert_not_ok(ret, "get_status should fail");
	zassert_equal(ret, -EINVAL, "The return status should be -EINVAL, got %d", ret);
}

ZTEST(temp_alarm_invalid_input, test_init_without_device)
{
	/* TODO(l8-task2): init without a sensor device is rejected.
	 * See TEST_SPEC.md "Suite temp_alarm_invalid_input" #2.
	 */
	int ret;

	ret = temp_alarm_init(NULL, DEFAULT_THRESHOLD);
	zassert_not_ok(ret, "init should fail");
	zassert_equal(ret, -ENODEV, "The return status should be -ENODEV, got %d", ret);
}

ZTEST(temp_alarm_invalid_input, test_init_threshold_out_of_range)
{
	/* TODO(l8-task2): init rejects thresholds outside the allowed range.
	 * See TEST_SPEC.md "Suite temp_alarm_invalid_input" #3.
	 */
	int ret;
	int thresholds[] = {-1, 101};

	for(uint8_t i = 0; i < ARRAY_SIZE(thresholds); i++) {
	    ret = temp_alarm_init(&fake_sensor_dev, thresholds[i]);
		zassert_not_ok(ret, "init should fail");
		zassert_equal(ret, -EINVAL, "The return status should be -EINVAL, got %d", ret);
	}
}

ZTEST(temp_alarm_invalid_input, test_set_threshold_out_of_range)
{
	/* TODO(l8-task2): set_threshold rejects values outside the allowed range,
	 * and keeps the old threshold.
	 * See TEST_SPEC.md "Suite temp_alarm_invalid_input" #4.
	 */
	int ret;
	int thresholds[] = {-1, 101};
	struct temp_alarm_status status;

	// In order to verify that invalid values passed to set_threshold() do not modify the
	// current threshold of 30°C, I will take advantage of the function set_threshold(),
	// which resets the alarm when a valid threshold is set. So, if the alarm is triggered with
	// a threshold of 30°C, and 2 invalid values are passed to set_threshold(), then the
	// alarm should never be cleared. I will set a temperature of 31°C (next int immediately
	// after 30), then set the 2 invalid values, and finally check that is_alarming is
	// still is_alarming == true
	sensor_fake_set_temperature(DEFAULT_THRESHOLD+1);
	ret = temp_alarm_check();
	zassert_ok(ret, "Check should succeed, but got error %d", ret);
	ret = temp_alarm_get_status(&status);
	zassert_ok(ret, "get_status should succeed");
	zassert_true(status.is_alarming, "is_alarming must be true");

	// The 2 invalid threshold are passed to set_threshold()
	for(uint8_t i = 0; i < ARRAY_SIZE(thresholds); i++) {
	    ret = temp_alarm_set_threshold(thresholds[i]);
		zassert_not_ok(ret, "set_threshold should fail with value %d °C", i);
		zassert_equal(ret, -EINVAL, "The return status should be -EINVAL, got %d", ret);
	}

	// After the rejected set_thresholds, the threshold should still be 30. The temperature
	// of 31 is set again, the temperature is checked and the status.is_alarming should be true,
	// because the original threshold of 30 should not have changed
	sensor_fake_set_temperature(DEFAULT_THRESHOLD+1);
	ret = temp_alarm_check();
	zassert_ok(ret, "Check should succeed, but got error %d", ret);
	ret = temp_alarm_get_status(&status);
	zassert_ok(ret, "get_status should succeed");
	zassert_true(status.is_alarming, "is_alarming should still be true");
}

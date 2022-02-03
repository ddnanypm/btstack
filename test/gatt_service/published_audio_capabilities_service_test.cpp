
// *****************************************************************************
//
// test battery service
//
// *****************************************************************************


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTestExt/MockSupport.h"

#include "hci.h"
#include "btstack_util.h"
#include "bluetooth.h"
#include "bluetooth_gatt.h"

#include "ble/gatt-service/published_audio_capabilities_service_server.h"
#include "published_audio_capabilities_service_test.h"
#include "mock_att_server.h"

static uint8_t my_codec_id[] = {0,1,2,3,4};
const uint8_t  my_value[] = {1,2,3};
static pacs_codec_specific_capability_t my_capability = {
       PACS_CODEC_SPECIFIC_CAPABILITY_TYPE_SAMPLING_FREQUENCY,
       3,
       my_value
    };
const uint8_t my_metadata[] = {1,2,3,4};

static pacs_record_t sink_record_0 = {
    my_codec_id,
    10,
    &my_capability,
    1,
    4,
    my_metadata
};


TEST_GROUP(PUBLISHED_AUDIO_CAPABILITIES_SERVICE_SERVER){ 
    att_service_handler_t * service; 
    uint16_t con_handle;
    uint16_t pacs_sinc_pac_handle_value;
    uint16_t pacs_sinc_pac_handle_client_configuration;
    pacs_record_t sink_pac_records[1];
    uint8_t sink_pac_records_num;

    void setup(void){
        // setup database
        att_set_db(profile_data);
        pacs_sinc_pac_handle_value = gatt_server_get_value_handle_for_characteristic_with_uuid16(0, 0xffff, ORG_BLUETOOTH_CHARACTERISTIC_SINK_PAC);
        pacs_sinc_pac_handle_client_configuration = gatt_server_get_client_configuration_handle_for_characteristic_with_uuid16(0, 0xffff, ORG_BLUETOOTH_CHARACTERISTIC_SINK_PAC);
        sink_pac_records_num = 1;
        sink_pac_records[0] = sink_record_0;
        // setup battery service
        published_audio_capabilities_service_server_init(sink_pac_records, sink_pac_records_num, NULL, 0);

        service = mock_att_server_get_service();
        con_handle = 0x00;
    }

    void teardown(){
        mock_deinit();
    }
};


TEST(PUBLISHED_AUDIO_CAPABILITIES_SERVICE_SERVER, lookup_attribute_handles){
    CHECK(pacs_sinc_pac_handle_value != 0);
    CHECK(pacs_sinc_pac_handle_client_configuration != 0);
}


TEST(PUBLISHED_AUDIO_CAPABILITIES_SERVICE_SERVER, read_battery_value){
    uint8_t response[2];
    uint16_t response_len;

    // invalid attribute handle
    response_len = mock_att_service_read_callback(con_handle, 0xffff, 0xffff, response, sizeof(response));
    CHECK_EQUAL(0, response_len);

    response_len = mock_att_service_read_callback(con_handle, pacs_sinc_pac_handle_client_configuration, 0, response, sizeof(response));
    CHECK_EQUAL(2, response_len);

    // response_len = mock_att_service_read_callback(con_handle, pacs_sinc_pac_handle_value, 0, response, sizeof(response));
    // CHECK_EQUAL(1, response_len);
}


int main (int argc, const char * argv[]){
    return CommandLineTestRunner::RunAllTests(argc, argv);
}

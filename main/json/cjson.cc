/*
 * cjson.cc
 *
 *  Created on: 23 de jan. de 2022
 *      Author: gabriel
 */

#include "cjson.h"

json_pack_status_e json_pack_data(uint8_t *img, uint32_t size, uint8_t *out){
	/*
	cJSON *payload = cJSON_CreateObject();

    cJSON *data = cJSON_CreateArray();
    for(uint32_t i = 0; i<size; i++){
    	cJSON_AddItemToArray(data, cJSON_CreateNumber(img[i]));
    }
    cJSON *id = cJSON_CreateNumber(0);

    cJSON_AddItemToObject(payload, "id", id);
    cJSON_AddItemToObject(payload, "img", data);

    out = cJSON_Print(payload);
    cJSON_Delete(payload);

    */

	return JSON_PACK_SUCCESS;
}



/*
 * cjson.h
 *
 *  Created on: 23 de jan. de 2022
 *      Author: gabriel
 */

#ifndef MAIN_JSON_CJSON_H_
#define MAIN_JSON_CJSON_H_

#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "cJSON.h"
#include "cJSON_Utils.h"

typedef enum {
	JSON_PACK_SUCCESS,
	JSON_PACK_FAIL
} json_pack_status_e;

json_pack_status_e json_pack_data(uint8_t *img, uint32_t size, uint8_t *out);

#endif /* MAIN_JSON_CJSON_H_ */

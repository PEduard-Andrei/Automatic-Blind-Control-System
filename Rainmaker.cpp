#include "Rainmaker.h"
#include "Buton.h"
#include "Potentiometru.h"
#include "Motor.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include "RMaker.h"

static void jaluzea_CreateNode();
static void jaluzea_CreateDevice();
static void jaluzea_CreateParameter();

static Node   	jaluzea_Node;
static Device 	*jaluzea_Device 		= NULL;
static Param 	*operationMode_Param 	= NULL;
static Param 	*position_Param 		= NULL;

const char * serviceNameProv = "PROV_JALUZEA";
const char * password_pop 	 = "12345678";

void SysProvEvent(arduino_event_t *sys_event)
{
    switch (sys_event->event_id)
    {
		case ARDUINO_EVENT_PROV_INIT:
			break;
		case ARDUINO_EVENT_PROV_START:
			WiFiProv.printQR(serviceNameProv, password_pop, "ble");
			break;
		case ARDUINO_EVENT_PROV_CRED_RECV:
			break;
		case ARDUINO_EVENT_WIFI_STA_GOT_IP:
			break;
		case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
			break;
		case ARDUINO_EVENT_PROV_CRED_FAIL:
			wifi_prov_mgr_reset_sm_state_on_failure();
			break;
		case ARDUINO_EVENT_PROV_CRED_SUCCESS:
			break;
		case ARDUINO_EVENT_PROV_END:
			break;
		default:
			break;
    }
}


/* de la rainmaker */
void rainmaker_write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
{
    const char *param_name = param->getParamName();

    if(strcmp(param_name, "Operation Mode") == 0)
    {
    	if(strcmp(val.val.s, "MANUAL") == 0)
    	{
    		operationMode = MANUAL;
    		Serial.println("Rainmaker mod operare : MANUAL");
    	}
    	else if(strcmp(val.val.s, "AUTO") == 0)
    	{
    		operationMode = AUTO;
    		Serial.println("Rainmaker mod operare : AUTO");
    	}
    }
    else if(strcmp(param_name, "Position") == 0)
    {
    	pozitieJaluzea = val.val.i;
    	vitezaMotor = INCET;
    	motor_update();
    }

    param->updateAndReport(val);
}

void rainmaker_init()
{
	uint8_t uuid[16] = {0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf, 0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02 };

	jaluzea_CreateNode();
	jaluzea_CreateDevice();
	jaluzea_CreateParameter();

	jaluzea_Device->addParam(*operationMode_Param);
	jaluzea_Device->addParam(*position_Param);
	jaluzea_Device->addCb(rainmaker_write_callback);

	jaluzea_Node.addDevice(*jaluzea_Device);

	RMaker.enableTZService();
	RMaker.enableSchedule();
	RMaker.enableScenes();
	RMaker.enableSystemService(SYSTEM_SERV_FLAGS_ALL);

	RMaker.start();

	WiFi.onEvent(SysProvEvent);
	WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, password_pop, serviceNameProv, NULL, uuid, false);
}

void rainmaker_main()
{

}

/* Catre rainmaker */
void rainmaker_update_and_report(int arg)
{
	param_val_t val;

	if(1 == arg)
	{
		if(MANUAL == operationMode)
		{
			val.type 	= RMAKER_VAL_TYPE_STRING;
			val.val.s 	= "MANUAL";
			esp_rmaker_raise_alert("Jaluzea Mod Manual");
		}
		else if(AUTO == operationMode)
		{
			val.type 	= RMAKER_VAL_TYPE_STRING;
			val.val.s 	= "AUTO";
			esp_rmaker_raise_alert("Jaluzea Mod Auto");
		}

		operationMode_Param->updateAndReport(val);
	}
	else if(2 == arg)
	{
		val.type 	= RMAKER_VAL_TYPE_INTEGER;
		val.val.i 	= pozitieJaluzea;
		position_Param->updateAndReport(val);
	}
}

static void jaluzea_CreateNode()
{
	jaluzea_Node = RMaker.initNode("Jaluzea Node", "Jaluzea Node Type");
}

static void jaluzea_CreateDevice()
{
	jaluzea_Device = new Device("Jaluzea", ESP_RMAKER_DEVICE_BLINDS_INTERNAL, NULL);
}

static void jaluzea_CreateParameter()
{
	operationMode_Param = new Param("Operation Mode", ESP_RMAKER_PARAM_MODE, value("MANUAL"), PROP_FLAG_READ | PROP_FLAG_WRITE | PROP_FLAG_PERSIST);
	static const char *operatingModes[] = {"MANUAL", "AUTO"};
	operationMode_Param->addValidStrList(operatingModes, 2);
	operationMode_Param->addUIType(ESP_RMAKER_UI_DROPDOWN);

	position_Param = new Param("Position", ESP_RMAKER_PARAM_SPEED, value(0), PROP_FLAG_READ | PROP_FLAG_WRITE | PROP_FLAG_PERSIST);
	position_Param->addBounds(value(0), value(100), value(1));
	position_Param->addUIType(ESP_RMAKER_UI_SLIDER);
}

#include "../inc/xmlConfig.h"


XmlConfig::XmlConfig()
{
	strcpy(ArduinoPort , "COM1");
	Delay = 0;
	CommFlush = 1;
	ThrottleMin = 0;
	ThrottleMax = 1024;
	ThrottleInvert = 0;
	PitchMin = 0;
	PitchMax = 1024;
	PitchInvert = 0;
	RollMin = 0;
	RollMax = 1024;
	RollInvert = 0;
	YawMin = 0;
	YawMax = 1024;
	YawInvert = 0;
	PropSpeedMin = 0;
	PropSpeedMax = 1024;
	PropSpeedInvert = 0;
	CarbHeatMin = 0;
	CarbHeatMax = 1024;
	CarbHeatInvert = 0;
	ParkBrakeMin = 0;
	ParkBrakeMax = 1024;
	ParkBrakeInvert = 0;
	ChokeMin = 0;
	ChokeMax = 1024;
	ChokeInvert = 0;
	LeftBrakeMin = 0;
	LeftBrakeMax = 1024;
	LeftBrakeInvert = 0;
	RightBrakeMin = 0;
	RightBrakeMax = 1024;
	RightBrakeInvert = 0;
}

void XmlConfig::Open(const char *filename)
{
	char line[1000];
	char *entry;

	//Open the config file
	FILE *fp = fopen(filename,"r");

	//If not opened, return a NULL
	if (!fp)
	{
		IsOpen = 0;
		return;
	}



	//Loop through XML
	while (!feof(fp))
	{
		//Readline
		fgets(line,sizeof(line),fp);

		//Determine node
		entry = strtok(line,"<>");
		entry = strtok(NULL,"<>");

		if (!entry)
		{
			break;
		}

		//Now determine what it contains
		if (strcmp(entry,"ArduinoPort") == 0)
		{
			entry = strtok(NULL,"<>");
			strcpy(ArduinoPort,entry);
		} 
		else if (strcmp(entry,"Delay") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&Delay);
		}
		else if (strcmp(entry,"Commflush") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&CommFlush);
		}
		else if (strcmp(entry,"ThrottleMin") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&ThrottleMin);
		}
		else if (strcmp(entry,"ThrottleMax") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&ThrottleMax);
		}
		else if (strcmp(entry,"ThrottleInvert") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&ThrottleInvert);
		}
		else if (strcmp(entry,"PitchMin") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&PitchMin);
		}
		else if (strcmp(entry,"PitchMax") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&PitchMax);
		}
		else if (strcmp(entry,"PitchInvert") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&PitchInvert);
		}
		else if (strcmp(entry,"RollMin") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&RollMin);
		}
		else if (strcmp(entry,"RollMax") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&RollMax);
		}
		else if (strcmp(entry,"RollInvert") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&RollInvert);
		}
		else if (strcmp(entry,"YawMin") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&YawMin);
		}
		else if (strcmp(entry,"YawMax") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&YawMax);
		}
		else if (strcmp(entry,"YawInvert") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&YawInvert);
		}
		else if (strcmp(entry,"LeftBrakeMin") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&LeftBrakeMin);
		}
		else if (strcmp(entry,"LeftBrakeMax") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&LeftBrakeMax);
		}
		else if (strcmp(entry,"LeftBrakeInvert") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&LeftBrakeInvert);
		}
		else if (strcmp(entry,"RightBrakeMin") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&RightBrakeMin);
		}
		else if (strcmp(entry,"RightBrakeMax") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&RightBrakeMax);
		}
		else if (strcmp(entry,"RightBrakeInvert") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&RightBrakeInvert);
		}
		else if (strcmp(entry,"PropSpeedMin") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&PropSpeedMin);
		}
		else if (strcmp(entry,"PropSpeedMax") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&PropSpeedMax);
		}
		else if (strcmp(entry,"PropSpeedInvert") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&PropSpeedInvert);
		}
		else if (strcmp(entry,"CarbHeatMin") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&CarbHeatMin);
		}
		else if (strcmp(entry,"CarbHeatMax") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&CarbHeatMax);
		}
		else if (strcmp(entry,"CarbHeatInvert") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&CarbHeatInvert);
		}
		else if (strcmp(entry,"ParkBrakeMin") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&ParkBrakeMin);
		}
		else if (strcmp(entry,"ParkBrakeMax") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&ParkBrakeMax);
		}
		else if (strcmp(entry,"ParkBrakeInvert") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&ParkBrakeInvert);
		}
		else if (strcmp(entry,"ChokeMin") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&ChokeMin);
		}
		else if (strcmp(entry,"ChokeMax") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&ChokeMax);
		}
		else if (strcmp(entry,"ChokeInvert") == 0)
		{
			entry = strtok(NULL,"<>");
			sscanf(entry,"%d",&ChokeInvert);
		}
	}

	IsOpen = 1;
}
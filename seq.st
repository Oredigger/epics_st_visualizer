program LRU24_SHOTPLAN


ss {X}{

	state off{
		entry{
			printf("Entered Off State\n");
		}
		when(SSPP_SHOT_PLAN_EXECUTE == 1){
			printf("Going to Run Plan from Off State\n");
			stepIndex = 0;	
								
		} state runPlan
	}

	state runPlan{
		entry{
		printf("Entered runPlan State\n");
		}
		when(SSPP_SHOT_PLAN_EXECUTE == 1 && execute[stepIndex] == 1 && stepIndex < 10){
			printf("Going to Run Shot from Run Plan State\n");

			CLRU08_LRU01_LRU_ACTIVE = lru01Active[stepIndex];
			pvPut(CLRU08_LRU01_LRU_ACTIVE, SYNC);
			CLRU08_LRU02_LRU_ACTIVE = lru02Active[stepIndex];
			pvPut(CLRU08_LRU02_LRU_ACTIVE, SYNC);
			CLRU08_LRU03_LRU_ACTIVE = lru03Active[stepIndex];
			pvPut(CLRU08_LRU03_LRU_ACTIVE, SYNC);
			CLRU08_LRU04_LRU_ACTIVE = lru04Active[stepIndex];
			pvPut(CLRU08_LRU04_LRU_ACTIVE, SYNC);
			CLRU08_LRU05_LRU_ACTIVE = lru05Active[stepIndex];
			pvPut(CLRU08_LRU05_LRU_ACTIVE, SYNC);
			CLRU08_LRU06_LRU_ACTIVE = lru06Active[stepIndex];
			pvPut(CLRU08_LRU06_LRU_ACTIVE, SYNC);
			CLRU08_LRU07_LRU_ACTIVE = lru07Active[stepIndex];
			pvPut(CLRU08_LRU07_LRU_ACTIVE, SYNC);
			CLRU08_LRU08_LRU_ACTIVE = lru08Active[stepIndex];
			pvPut(CLRU08_LRU08_LRU_ACTIVE, SYNC);
			CLRU08_LRU09_LRU_ACTIVE = lru09Active[stepIndex];
			pvPut(CLRU08_LRU09_LRU_ACTIVE, SYNC);
			CLRU08_LRU10_LRU_ACTIVE = lru10Active[stepIndex];
			pvPut(CLRU08_LRU10_LRU_ACTIVE, SYNC);
			CLRU08_LRU11_LRU_ACTIVE = lru11Active[stepIndex];
			pvPut(CLRU08_LRU11_LRU_ACTIVE, SYNC);
			CLRU08_LRU12_LRU_ACTIVE = lru12Active[stepIndex];
			pvPut(CLRU08_LRU12_LRU_ACTIVE, SYNC);
			CLRU08_LRU13_LRU_ACTIVE = lru13Active[stepIndex];
			pvPut(CLRU08_LRU13_LRU_ACTIVE, SYNC);
			CLRU08_LRU14_LRU_ACTIVE = lru14Active[stepIndex];
			pvPut(CLRU08_LRU14_LRU_ACTIVE, SYNC);
			CLRU08_LRU15_LRU_ACTIVE = lru15Active[stepIndex];
			pvPut(CLRU08_LRU15_LRU_ACTIVE, SYNC);
			CLRU08_LRU16_LRU_ACTIVE = lru16Active[stepIndex];
			pvPut(CLRU08_LRU16_LRU_ACTIVE, SYNC);
			CLRU08_LRU17_LRU_ACTIVE = lru17Active[stepIndex];
			pvPut(CLRU08_LRU17_LRU_ACTIVE, SYNC);
			CLRU08_LRU18_LRU_ACTIVE = lru18Active[stepIndex];
			pvPut(CLRU08_LRU18_LRU_ACTIVE, SYNC);
			CLRU08_LRU19_LRU_ACTIVE = lru19Active[stepIndex];
			pvPut(CLRU08_LRU19_LRU_ACTIVE, SYNC);
			CLRU08_LRU20_LRU_ACTIVE = lru20Active[stepIndex];
			pvPut(CLRU08_LRU20_LRU_ACTIVE, SYNC);
			CLRU08_LRU21_LRU_ACTIVE = lru21Active[stepIndex];
			pvPut(CLRU08_LRU21_LRU_ACTIVE, SYNC);
			CLRU08_LRU22_LRU_ACTIVE = lru22Active[stepIndex];
			pvPut(CLRU08_LRU22_LRU_ACTIVE, SYNC);
			CLRU08_LRU23_LRU_ACTIVE = lru23Active[stepIndex];
			pvPut(CLRU08_LRU23_LRU_ACTIVE, SYNC);
			CLRU08_LRU24_LRU_ACTIVE = lru24Active[stepIndex];
			pvPut(CLRU08_LRU24_LRU_ACTIVE, SYNC);

			//Transfers File Step[i] into Cluster Setup and uploads
			strcpy(CLRU08_TMG_DPG_BULK_UPLOAD_FILEPATH, pattern[stepIndex]);
			pvPut(CLRU08_TMG_DPG_BULK_UPLOAD_FILEPATH, SYNC);

			CLRU08_TMG_DPG_PATTERN_UPLOAD_MODE = 1;
			pvPut(CLRU08_TMG_DPG_PATTERN_UPLOAD_MODE, SYNC);	

			CLRU08_TMG_DPG_PATTERN_UPLOAD_START = 1;
			pvPut(CLRU08_TMG_DPG_PATTERN_UPLOAD_START, SYNC);	

			//Transfers Repeater Step[i] and uploads
			SSPP_SHOT_SEQ_INTERVAL = interval[stepIndex];
			pvPut(SSPP_SHOT_SEQ_INTERVAL, SYNC);

			SSPP_SHOT_SEQ_SHOTS_REQUESTED = shots[stepIndex];
			pvPut(SSPP_SHOT_SEQ_SHOTS_REQUESTED, SYNC);

			SSPP_SHOT_SETUP_HV_SETPOINT = setpoint[stepIndex];
			pvPut(SSPP_SHOT_SETUP_HV_SETPOINT, SYNC);

			SSPP_SHOT_SETUP_APPLY = 1;
			pvPut(SSPP_SHOT_SETUP_APPLY, SYNC);
	


		}state runShot


		when(SSPP_SHOT_PLAN_EXECUTE == 1 && execute[stepIndex] == 0 && stepIndex < 10){
				
			stepIndex = stepIndex + 1;


		}state runPlan

		when(stepIndex >= 10){

			SSPP_SHOT_PLAN_EXECUTE = 0;
			pvPut(SSPP_SHOT_PLAN_EXECUTE, SYNC);
				
	
		}state off





	}

	state runShot{
		entry{
			printf("Entered runShot State\n");
		}
		when(SSPP_SHOT_PLAN_EXECUTE == 1 && SSPP_SHOT_SETUP_STEP_STATUS_FLAG == 1 && CLRU08_LRU01_DPG_USER_STATUS == 3){
			
			SSPP_SEQ_DATA_SELECT = 1;
			pvPut(SSPP_SEQ_DATA_SELECT, SYNC);

		}state running

	}

	state running{
		entry{
			printf("Entered Running\n");
		}
		
		when(SSPP_SEQ_DATA_SELECT == 0){
			stepIndex = stepIndex + 1;		
		}state runPlan

	}

}


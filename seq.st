ss volt_check {
    state light_off 
    {
        when (voltage > 5.0)
        {
            light = TRUE;
            pvPut(light);
        } state light_on
    }

    state light_on
    {
        when (voltage < 3.0)
        {
            light = FALSE;
            pvPut(light);
        } state light_off
    }
}
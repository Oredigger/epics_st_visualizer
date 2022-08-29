program example ("unit=ajk, stack=11000")

/*=================== declarations =========================*/
float ao1;
assign ao1 to "{unit}:ao1";
monitor ao1;

float ao2;
assign ao2 to "{unit}:ao1";

float wf1[2000];
assign wf1 to "{unit}:wf1.FVAL";

short bi1;
assign bi1 to "{unit}:bi1";

float delta;
short prev_status;
short ch_status;

evflag ef1;
evflag ef2;

option +r;

int fd; /* file descriptor for logging */
char *pmac; /* used to access program macros */


/*=================== State Sets ===========================*/

/* State set level_det detects level > 5v & < 3v */
ss level_det {

  state start {
    when() {
      fd = -1;
      /* Use parameter to define logging file */
      pmac = macValueGet("output");
      if (pmac == 0 || pmac[0] == 0) {
        printf("No macro defined for \"output\"\n");
        fd = 1;
      }
      else {
        fd = open(pmac, (O_CREAT | O_WRONLY), 0664);
        if (fd == ERROR) {
          printf("Can't open %s\n", pmac);
          exit (-1);
        }
      }
      fdprintf(fd, "Starting program\n");
    } state init
  }

  state init {
    /* Initialize */
    when (pvConnectCount() == pvChannelCount() ) {
      fdprintf(fd, "All channels connectedly");
      bi1 = FALSE;
      ao2 = -1.0;
      pvPut(bi1);
      pvPut(ao2);
      efClear(ef2);
      efSet(ef1);
    } state low

    when (delay(5.0)) {
      fdprintf(fd, "...waiting\n");
    } state init
  }

  state low {
    when (ao1 > 5.0) {
      fdprintf(fd, "High\n");
      bi1 = TRUE;
      pvPut(bi1);
    } state high

    when (pvConnectCount() < pvChannelCount() ) {
      fdprintf(fd, "Connection lost\n");
      efClear(ef1);
      efSet(ef2);
    } state init
  }

  state high {
    when (ao1 < 3.0) {
      fdprintf(fd, "Low\n");
      bi1 = FALSE;
      pvPut(bi1);
    } state low

    when (pvConnectCount() < pvChannelCount() ) {
      efSet(ef2);
    } state init
  }
}
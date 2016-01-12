#ifndef IntervalType_h
#define IntervalType_h

typedef struct {
  unsigned long interval;
  unsigned long lastTick;
  unsigned long count;
} IntervalType;

void execInInterval(unsigned long m, IntervalType* i, void (*fn)()) {
  if (m >= i->lastTick + i->interval) {
    i->lastTick = m;
    i->count++;
    fn();
  }
}

#endif

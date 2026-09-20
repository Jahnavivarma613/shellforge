#ifndef EXPAND_H
#define EXPAND_H

#include "parser.h"

void expand_command(Command *cmd);

void expand_pipeline(Pipeline *pipeline);

#endif

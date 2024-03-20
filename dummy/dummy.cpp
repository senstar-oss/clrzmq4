#include <zmq.h>
#include "dummy.h"

int main()
{
	void* context = zmq_ctx_new();
	zmq_ctx_destroy(context);
}

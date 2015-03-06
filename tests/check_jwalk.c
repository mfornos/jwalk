#include <stdio.h>
#include <assert.h>

#include "../src/jwalk.h"

int main(void)
{
	int res = json_walk("assets/inputs/servlet.json", "web-app.*.servlet-name", '_');
    assert(res == 5);
    json_walker_t js = json_walker();
    assert(js.delimiter == '_');
    return 0;
}

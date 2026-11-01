#ifndef PLUG_H_
#define PLUG_H_

#define LIST_OF_PLUGS \
    PLUG(plug_init, void, void) \
    PLUG(plug_load_resource, void*, const char*, size_t*) \
    PLUG(plug_free_resource, void, void*) \
    PLUG(plug_update, void, void)

#define PLUG(name, ret, ...) ret name(__VA_ARGS__);
LIST_OF_PLUGS
#undef PLUG

#endif // PLUG_H_

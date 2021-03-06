#include "query-test.h"
#include "c_test_builder.h"
#include "c_test_objects.h"
#include <assert.h>

int testQueryBuilderError(OBX_store* store, OBX_cursor* cursor, obx_schema_id entity_id) {
    int rc = 0;
    OBX_query_builder* builder = obx_qb_create(store, entity_id);
    assert(builder);

    obx_schema_id entity_prop_id = obx_store_entity_property_id(store, entity_id, "id");
    assert(entity_prop_id);

    // comparing id (long) with a string
    obx_qb_cond cond = obx_qb_string_equal(builder, entity_prop_id, "aaa", true);
    assert(0 == cond);
    assert(OBX_ERROR_PROPERTY_TYPE_MISMATCH == obx_qb_error_code(builder));

    // check that the message is same
    const char * msg1 = obx_last_error_message();
    const char * msg2 = obx_qb_error_message(builder);
    assert(msg1);
    assert(msg2);
    assert(strcmp(msg1, msg2) == 0);

    // this should not create a query
    OBX_query* query = obx_query_create(builder);
    assert(query == NULL);

    obx_qb_close(builder);
    obx_query_close(query);
    return rc;
}

void checkFooItem(void* data, obx_id id, const char* text) {
    Foo_table_t foo1 = Foo_as_root(data);
    assert(Foo_id(foo1) == id);
    assert(strcmp(Foo_text(foo1), text) == 0);
}

int testQueryBuilderEqual(OBX_store* store, OBX_cursor* cursor, obx_schema_id entity_id) {
    int rc;

    if ((rc = obx_cursor_remove_all(cursor))) goto err;

    obx_id id1 = 0, id2 = 0, id3 = 0;
    if ((rc = put_foo(cursor, &id1, "aaa"))) goto err;
    if ((rc = put_foo(cursor, &id2, "AAA"))) goto err;
    if ((rc = put_foo(cursor, &id3, "aaa"))) goto err;

    {   // STRING case sensitive
        OBX_query_builder* builder = obx_qb_create(store, entity_id);
        assert(builder);

        obx_qb_string_equal(builder, FOO_prop_text, "aaa", true);

        OBX_query* query = obx_query_create(builder);
        assert(query);

        OBX_bytes_array* items = obx_query_find(query, cursor);
        assert(items);
        assert(items->count == 2);
        checkFooItem(items->bytes[0].data, id1, "aaa");
        checkFooItem(items->bytes[1].data, id3, "aaa");
        obx_bytes_array_free(items);

        obx_qb_close(builder);
        obx_query_close(query);
    }


    {   // STRING case insensitive
        OBX_query_builder* builder = obx_qb_create(store, entity_id);
        assert(builder);

        obx_qb_string_equal(builder, FOO_prop_text, "aaa", false);

        OBX_query* query = obx_query_create(builder);
        assert(query);

        OBX_bytes_array* items = obx_query_find(query, cursor);
        assert(items);
        assert(items->count == 3);
        checkFooItem(items->bytes[0].data, id1, "aaa");
        checkFooItem(items->bytes[1].data, id2, "AAA");
        checkFooItem(items->bytes[2].data, id3, "aaa");
        obx_bytes_array_free(items);

        obx_qb_close(builder);
        obx_query_close(query);
    }

    {   // STRING no-match
        OBX_query_builder* builder = obx_qb_create(store, entity_id);
        assert(builder);

        obx_qb_string_equal(builder, FOO_prop_text, "you-wont-find-me", true);

        OBX_query* query = obx_query_create(builder);
        assert(query);

        OBX_bytes_array* items = obx_query_find(query, cursor);
        assert(items);
        assert(items->count == 0);
        obx_bytes_array_free(items);

        obx_qb_close(builder);
        obx_query_close(query);
    }

    {   // LONG
        OBX_query_builder* builder = obx_qb_create(store, entity_id);
        assert(builder);

        obx_qb_int_equal(builder, FOO_prop_id, id3);

        OBX_query* query = obx_query_create(builder);
        assert(query);

        OBX_bytes_array* items = obx_query_find(query, cursor);
        assert(items);
        assert(items->count == 1);
        checkFooItem(items->bytes[0].data, id3, "aaa");
        obx_bytes_array_free(items);

        obx_qb_close(builder);
        obx_query_close(query);
    }

    {   // LONG no-match
        OBX_query_builder* builder = obx_qb_create(store, entity_id);
        assert(builder);

        obx_qb_int_equal(builder, FOO_prop_id, -1);

        OBX_query* query = obx_query_create(builder);
        assert(query);

        OBX_bytes_array* items = obx_query_find(query, cursor);
        assert(items);
        assert(items->count == 0);
        obx_bytes_array_free(items);

        obx_qb_close(builder);
        obx_query_close(query);
    }

    return 0;

    err:
    printf("%s error: %d", __FUNCTION__, rc);
    return rc;
}

int testQueryBuilderBetween(OBX_store* store, OBX_cursor* cursor, obx_schema_id entity_id) {
    int rc;

    if ((rc = obx_cursor_remove_all(cursor))) goto err;

    obx_id id1 = 0, id2 = 0, id3 = 0;
    if ((rc = put_foo(cursor, &id1, "aaa"))) goto err;
    if ((rc = put_foo(cursor, &id2, "AAA"))) goto err;
    if ((rc = put_foo(cursor, &id3, "aaa"))) goto err;

    {   // 2 and 3
        OBX_query_builder* builder = obx_qb_create(store, entity_id);
        assert(builder);

        obx_qb_int_between(builder, FOO_prop_id, id2, id3);

        OBX_query* query = obx_query_create(builder);
        assert(query);

        OBX_bytes_array* items = obx_query_find(query, cursor);
        assert(items);
        assert(items->count == 2);
        checkFooItem(items->bytes[0].data, id2, "AAA");
        checkFooItem(items->bytes[1].data, id3, "aaa");
        obx_bytes_array_free(items);

        obx_qb_close(builder);
        obx_query_close(query);
    }

    {   // 2 only
        OBX_query_builder* builder = obx_qb_create(store, entity_id);
        assert(builder);

        obx_qb_int_between(builder, FOO_prop_id, id2, id2);

        OBX_query* query = obx_query_create(builder);
        assert(query);

        OBX_bytes_array* items = obx_query_find(query, cursor);
        assert(items);
        assert(items->count == 1);
        checkFooItem(items->bytes[0].data, id2, "AAA");
        obx_bytes_array_free(items);

        obx_qb_close(builder);
        obx_query_close(query);
    }

    {   // 2 and 3
        OBX_query_builder* builder = obx_qb_create(store, entity_id);
        assert(builder);

        obx_qb_int_between(builder, FOO_prop_id, id3, id2);

        OBX_query* query = obx_query_create(builder);
        assert(query);

        OBX_bytes_array* items = obx_query_find(query, cursor);
        assert(items);
        assert(items->count == 2);
        checkFooItem(items->bytes[0].data, id2, "AAA");
        checkFooItem(items->bytes[1].data, id3, "aaa");
        obx_bytes_array_free(items);

        obx_qb_close(builder);
        obx_query_close(query);
    }

    return 0;

    err:
    printf("%s error: %d", __FUNCTION__, rc);
    return rc;
}

int testQueryBasics(OBX_store* store, OBX_cursor* cursor, obx_schema_id entity_id) {
    int rc;

    if ((rc = obx_cursor_remove_all(cursor))) goto err;

    obx_id id1 = 0, id2 = 0, id3 = 0, id4 = 0;
    if ((rc = put_foo(cursor, &id1, "aaa"))) goto err;
    if ((rc = put_foo(cursor, &id2, "AAA"))) goto err;
    if ((rc = put_foo(cursor, &id3, "bbb"))) goto err;
    if ((rc = put_foo(cursor, &id4, "aaa"))) goto err;

    {   // count
        OBX_query_builder* builder = obx_qb_create(store, entity_id);
        assert(builder);

        OBX_query* query = obx_query_create(builder);
        assert(query);

        uint64_t count = 0;
        if ((rc = obx_query_count(query, cursor, &count))) goto err;
        assert(count == 4);

        obx_qb_close(builder);
        obx_query_close(query);
    }

    {   // find
        OBX_query_builder* builder = obx_qb_create(store, entity_id);
        assert(builder);

        OBX_query* query = obx_query_create(builder);
        assert(query);

        OBX_bytes_array* items = obx_query_find(query, cursor);
        assert(items);
        assert(items->count == 4);

        obx_bytes_array_free(items);
        obx_qb_close(builder);
        obx_query_close(query);
    }

    {   // remove
        OBX_query_builder* builder = obx_qb_create(store, entity_id);
        assert(builder);

        obx_qb_string_equal(builder, FOO_prop_text, "aaa", true);

        OBX_query* query = obx_query_create(builder);
        assert(query);

        uint64_t count = 0;
        if ((rc = obx_query_remove(query, cursor, &count))) goto err;
        assert(count == 2);

        obx_qb_close(builder);
        obx_query_close(query);
    }

    {   // find_keys
        OBX_query_builder* builder = obx_qb_create(store, entity_id);
        assert(builder);

        OBX_query* query = obx_query_create(builder);
        assert(query);

        OBX_id_array* keys = obx_query_find_ids(query, cursor);
        assert(keys);
        assert(keys->count == 2);

        obx_id_array_free(keys);
        obx_qb_close(builder);
        obx_query_close(query);
    }

    return 0;

    err:
    printf("%s error: %d", __FUNCTION__, rc);
    return rc;
}

int testQueries(OBX_store* store, OBX_cursor* cursor) {
    int rc = 0;

    obx_schema_id entity_id = obx_store_entity_id(store, "Foo");
    if (!entity_id) return obx_last_error_code();

    if ((rc = testQueryBuilderError(store, cursor, entity_id))) return rc;
    if ((rc = testQueryBuilderEqual(store, cursor, entity_id))) return rc;
    if ((rc = testQueryBuilderBetween(store, cursor, entity_id))) return rc;
    if ((rc = testQueryBasics(store, cursor, entity_id))) return rc;
    return rc;
}
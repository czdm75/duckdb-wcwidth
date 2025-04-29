#define DUCKDB_EXTENSION_MAIN

#include "render_width_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension_util.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>

#include "duckdb/function/udf_function.hpp"
#include "utf8proc.hpp"
#include "utf8proc_wrapper.hpp"

namespace duckdb {

// See duckdb thirdparty/utf8proc/utf8proc_wrapper.cpp
static inline int32_t RenderWidth(string_t str) {
    const size_t size = str.GetSize();
    const char *str_data = str.GetData();

    int32_t render_width = 0;
    size_t pos = 0;
    while (pos < size) {
        int sz;
        auto codepoint = Utf8Proc::UTF8ToCodepoint(str_data + pos, sz);
        auto properties = utf8proc_get_property(codepoint);
        render_width += properties->charwidth;
        pos += sz;
    }
    return render_width;
}

static void LoadInternal(DatabaseInstance &instance) {
    // Register a scalar function
    scalar_function_t function = UDFWrapper::CreateScalarFunction("render_width", {LogicalType::VARCHAR}, LogicalType::INTEGER, RenderWidth);
    ScalarFunction render_width_scalar_function = ScalarFunction("render_width", {LogicalType::VARCHAR}, LogicalType::INTEGER, function);
    ExtensionUtil::RegisterFunction(instance, render_width_scalar_function);
}

void RenderWidthExtension::Load(DuckDB &db) {
	LoadInternal(*db.instance);
}
std::string RenderWidthExtension::Name() {
	return "render_width";
}

std::string RenderWidthExtension::Version() const {
#ifdef EXT_VERSION_RENDER_WIDTH
	return EXT_VERSION_RENDER_WIDTH;
#else
	return "";
#endif
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void render_width_init(duckdb::DatabaseInstance &db) {
    duckdb::DuckDB db_wrapper(db);
    db_wrapper.LoadExtension<duckdb::RenderWidthExtension>();
}

DUCKDB_EXTENSION_API const char *render_width_version() {
	return duckdb::DuckDB::LibraryVersion();
}
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif

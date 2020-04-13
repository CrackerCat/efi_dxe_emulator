#include "guids.h"
#include "efi_definitions.h"

#include <csv.h>
#include <string>
#include <map>

/**
 * Required to avail EFI_GUID to be used as the key type in std::map.
 */
struct guid_comparator
{
    bool operator()(const EFI_GUID& left, const EFI_GUID& right) const
    {
        return memcmp(&left, &right, sizeof(EFI_GUID)) < 0;
    }
};

static std::map<EFI_GUID, std::string, guid_comparator> guid_db;

int
load_guids(char* guids_file)
{
    try
    {
        io::CSVReader<2> in(guids_file);

        std::string guid, friendly_name;
        while (in.read_row(guid, friendly_name)) {
            guid_db[string_to_guid(guid.c_str())] = friendly_name;
        }

        return 0;
    }
    catch (const std::exception&)
    {
        return -1;
    }
}

const char*
get_guid_friendly_name(const EFI_GUID& guid)
{
    try
    {
        return guid_db.at(guid).c_str();
    }
    catch (const std::out_of_range&)
    {
        return "Unknown GUID";
    }
}

char*
guid_to_string(EFI_GUID* guid)
{
    static char guid_str[37] = { 0 };
    snprintf(guid_str, sizeof(guid_str), "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        guid->Data1, guid->Data2, guid->Data3,
        guid->Data4[0], guid->Data4[1], guid->Data4[2], guid->Data4[3],
        guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7]);
    return guid_str;
}

EFI_GUID string_to_guid(const char * str)
{
    EFI_GUID guid;
    sscanf(str,
        "%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
        &guid.Data1, &guid.Data2, &guid.Data3,
        &guid.Data4[0], &guid.Data4[1], &guid.Data4[2], &guid.Data4[3],
        &guid.Data4[4], &guid.Data4[5], &guid.Data4[6], &guid.Data4[7]);

    return guid;
}

#define PCD_DATABASE_HOB_GUID \
  { \
    0xEA296D92, 0x0B69, 0x423C, { 0x8C, 0x28, 0x33, 0xB4, 0xE0, 0xA9, 0x12, 0x68 } \
  }

#define HOB_LIST_GUID \
  { \
    0x7739f24c, 0x93d7, 0x11d4, {0x9a, 0x3a, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d } \
  }

EFI_GUID gPcdDataBaseHobGuid = PCD_DATABASE_HOB_GUID;

EFI_GUID gEfiHobListGuid = HOB_LIST_GUID;
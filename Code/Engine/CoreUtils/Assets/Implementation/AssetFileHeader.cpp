#include <CoreUtils/PCH.h>
#include <CoreUtils/Assets/AssetFileHeader.h>

static const char* g_szAssetTag = "ezAsset";
ezUInt16 ezAssetFileHeader::s_uiSerializedHeaderSize = 0;

ezAssetFileHeader::ezAssetFileHeader()
{
  // initialize to a 'valid' hash
  // this may get stored, unless someone sets the hash
  m_uiHash = 0;
  m_uiVersion = 0;
}

enum ezAssetFileHeaderVersion : ezUInt8
{
  Version1 = 1,
  Version2,

  VersionCount,
  VersionCurrent = VersionCount - 1
};

void ezAssetFileHeader::Write(ezStreamWriter& stream) const
{
  EZ_ASSERT_DEBUG(m_uiHash != 0xFFFFFFFFFFFFFFFF, "Cannot write an invalid hash to file");

  // 8 Bytes for identification + version
  stream.WriteBytes(g_szAssetTag, 7);

  const ezUInt8 uiVersion = ezAssetFileHeaderVersion::VersionCurrent;
  stream << uiVersion;

  // 8 Bytes for the hash
  stream << m_uiHash;
  // 2 for the type version
  stream << m_uiVersion;
}

void ezAssetFileHeader::Read(ezStreamReader& stream)
{
  // initialize to 'invalid'
  m_uiHash = 0xFFFFFFFFFFFFFFFF;
  m_uiVersion = 0;

  char szTag[8] = { 0 };
  if (stream.ReadBytes(szTag, 7) < 7)
  {
    EZ_REPORT_FAILURE("The stream does not contain a valid asset file header");
    return;
  }

  szTag[7] = '\0';

  // invalid asset file ... this is not going to end well
  EZ_ASSERT_DEBUG(ezStringUtils::IsEqual(szTag, g_szAssetTag), "The stream does not contain a valid asset file header");

  if (!ezStringUtils::IsEqual(szTag, g_szAssetTag))
    return;

  ezUInt8 uiVersion = 0;
  stream >> uiVersion;

  ezUInt64 uiHash = 0;
  stream >> uiHash;

  // future version?
  EZ_ASSERT_DEV(uiVersion <= ezAssetFileHeaderVersion::VersionCurrent, "Unknown asset header version %u", uiVersion);

  if (uiVersion >= ezAssetFileHeaderVersion::Version2)
  {
    stream >> m_uiVersion;
  }

  // older version? set the hash to 'invalid'
  if (uiVersion != ezAssetFileHeaderVersion::VersionCurrent)
    return;

  m_uiHash = uiHash;
}

ezUInt16 ezAssetFileHeader::GetSerializedSize()
{
  if (s_uiSerializedHeaderSize != 0)
    return s_uiSerializedHeaderSize;

  // Serialize a dummy header to get the size it takes to store it.
  ezAssetFileHeader temp;
  temp.SetFileHashAndVersion(123, 1);
  ezMemoryStreamStorage storage;
  ezMemoryStreamWriter writer(&storage);
  temp.Write(writer);
  s_uiSerializedHeaderSize = (ezUInt16)storage.GetStorageSize();

  return s_uiSerializedHeaderSize;
}

EZ_STATICLINK_FILE(CoreUtils, CoreUtils_Assets_Implementation_AssetFileHeader);


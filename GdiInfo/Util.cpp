#include "StdAfx.h"
#include "Util.h"

CString GetGdiType(HGDIOBJ handle)
{
  CString retGdiType;
  DWORD gdiType = GetObjectType(handle);
  switch (gdiType)
  {
  case OBJ_PEN:
    retGdiType = _T("PEN");
    break;

  case OBJ_BRUSH:
    retGdiType = _T("BRUSH");
    break;

  case OBJ_DC:
    retGdiType = _T("DC");
    break;

  case OBJ_METADC:
    retGdiType = _T("METADC");
    break;

  case OBJ_PAL:
    retGdiType = _T("PAL");
    break;

  case OBJ_FONT:
    retGdiType = _T("FONT");
    break;

  case OBJ_BITMAP:
    retGdiType = _T("BITMAP");
    break;

  case OBJ_REGION:
    retGdiType = _T("REGION");
    break;

  case OBJ_METAFILE:
    retGdiType = _T("METAFILE");
    break;

  case OBJ_MEMDC:
    retGdiType = _T("MEMDC");
    break;

  case OBJ_EXTPEN:
    retGdiType = _T("EXTPEN");
    break;

  case OBJ_ENHMETADC:
    retGdiType = _T("NHMETADC");
    break;

  case OBJ_ENHMETAFILE:
    retGdiType = _T("ENHMETAFILE");
    break;

  default:
    break;
  }
  return retGdiType;

}
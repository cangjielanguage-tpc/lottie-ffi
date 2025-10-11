
package ohos_app_cangjie_entry


//internal import ohos.file_fs.*
internal import std.fs.*
internal import std.sync.*

internal import ohos.hilog.*
internal import ohos.system_date_time.SystemDateTime

internal import ohos.ability.*
internal import ohos.resource_manager.*
internal import ohos.bundle_manager.*


public func readFile(path: String): Array<Byte> {
    return File.readFrom(path)
}

public func loadPath(src: String, context: Option<AbilityContext>): Array<UInt8> {
    //获取rawfile的字节数组数据
    if (let Some(v) <- context) {
        let stageContext = getStageContext(v)
        let rm: ResourceManager = ResourceManager.getResourceManager(stageContext)
        try {
            let buffer: Array<UInt8> = rm.getRawFileContent(src)
            return buffer
        } catch (e: Exception) {
            Hilog.error(0, "lipei", "begin------------times = ${SystemDateTime.getCurrentTime(isNano: true)}")
        }
    }
    return Array<UInt8>()
}

public func loadMedia(resId: Int32, context: Option<AbilityContext>): Array<UInt8> {
    if (let Some(v) <- context) {
        let stageContext = getStageContext(v)
        let rm: ResourceManager = ResourceManager.getResourceManager(stageContext)
        return rm.getMediaContent(resId, 0)
    }
    ////Hilog.error(0, "", "[loadMedia]: the globalAbilityContext is None.")
    return Array<UInt8>()
}

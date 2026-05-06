meta:
	ADDON_NAME = ofxGrblKit
	ADDON_DESCRIPTION = Optional ofKitty integration kit for ofxGrbl.
	ADDON_AUTHOR = @gitbruno
	ADDON_TAGS = "addon" "grbl" "ecs" "imgui" "ofkitty" "machine"
	ADDON_URL = https://github.com/ofkitty/ofxGrblKit

common:
	ADDON_DEPENDENCIES = ofxGrbl ofxKit

	# Allow includes such as "ofxGrbl/src/..." and sibling addon headers.
	ADDON_INCLUDES += $(OF_ROOT)/addons

linux64:
vs:
linuxarmv6l:
linuxarmv7l:
android/armeabi:
android/armeabi-v7a:
osx:
ios:
tvos:

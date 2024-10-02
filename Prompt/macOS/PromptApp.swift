//
//  PromptApp.swift
//  Prompt
//
//  Created by MakeThingsSimple on 30/9/2024.
//

import OSLog
import SwiftUI

@main
struct PromptApp: App {
    static let logger = Logger()
    
    static func isDarkThemeEnabled() -> Bool {
#if os(macOS)
        let darkAppearance = NSAppearance(named: NSAppearance.Name.darkAqua)
        return NSApp.effectiveAppearance == darkAppearance
#endif
    }

#if os(macOS)
    private static let backgroundColor = NSColor.controlBackgroundColor
#endif
    
    static func getBackgroundColor() -> Color {
#if os(macOS)
        return .init(nsColor: PromptApp.backgroundColor)
#endif
    }
    
    static func getWindowBackgroundColorAsHex() -> String? {
#if os(macOS)
        let ciColor = CIColor(color: PromptApp.backgroundColor)!
        let red = UInt8(ciColor.red * 255)
        let green = UInt8(ciColor.green * 255)
        let blue = UInt8(ciColor.blue * 255)
        return .init(format: "#%02x%02x%02x", red, green, blue)
#endif
    }
    
    var body: some Scene {
        WindowGroup {
            EditorView()
        }
    }
}

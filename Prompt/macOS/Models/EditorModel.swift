//
//  EditorModel.swift
//  Prompt
//
//  Created by MakeThingsSimple on 30/9/2024.
//

import Foundation

extension EditorView {
    
    struct Model {
        enum Indentation: CustomStringConvertible {
            case spaces(UInt)
            case tabs(UInt)
            
            var description: String {
                if case .spaces(let numberOfSpaces) = self {
                    return "Spaces: \(numberOfSpaces)"
                } else if case .tabs(let tabWidth) = self {
                    return "Tabs: \(tabWidth)"
                }
                
                fatalError()
            }
        }
        
        enum Encoding: CaseIterable, CustomStringConvertible {
            case utf8
            
            var description: String {
                switch self {
                case .utf8:
                    return "UTF-8"
                }
            }
        }
        
        enum EndLine: CaseIterable, CustomStringConvertible {
            /// @brief UNIX end line encoding, where there is only one line feed (LF)
            case lf
            /// @brief Windows end line encoding, where there are first a carriage return (CR), then a line feed (LF)
            case crlf
            /// @brief Classic MacOS end line encoding, where there is only one carriage return (CR)
            case cr
            
            var description: String {
                switch self {
                case .lf:
                    return "LF"
                case .crlf:
                    return "CRLF"
                case .cr:
                    return "CR"
                }
            }
        }
        
        enum EditorType: CaseIterable, CustomStringConvertible {
            case simple
            case c, cpp, objectiveC, objectiveCpp
            
            var description: String {
                switch self {
                case .simple:
                    return "simple"
                case .c:
                    return "C"
                case .cpp:
                    return "C++"
                case .objectiveC:
                    return "Objective-C"
                case .objectiveCpp:
                    return "Objective-C++"
                }
            }
            
            /// @brief Return the editor type in the format expected by the Monaco Editor
            var monacoEditorLanguage: String {
                return self.description
            }
        }
        
        var modified = false
        var fileName: String? = nil
        var line: UInt = 1
        var col: UInt = 1
        var indentation = Indentation.spaces(4)
        var encoding = Encoding.utf8
        var endLine = EndLine.lf
        var type = EditorType.simple
    }
    
}

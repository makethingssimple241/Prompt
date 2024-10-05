//
//  EditorView.swift
//  Prompt
//
//  Created by MakeThingsSimple on 30/9/2024.
//

import SwiftUI

struct Toolbar {
    
    struct Button<Label: View>: View {
        @State var action: () -> Void
        @State var label: () -> Label
        
        @State private var isHovered = false
        
        var body: some View {
            SwiftUI.Button(action: action) {
                label()
                    .padding(5)
            }
            .onHover { hovering in
                isHovered = hovering
            }
            .buttonStyle(.plain)
            .background {
                if isHovered {
                    Color.accentColor
                }
            }
        }
    }
    
}

struct EditorView: View {
    @StateObject var vm = ViewModel()
    
    var body: some View {
        VStack(spacing: 5) {
            MonacoEditorView(vm: vm)
                .frame(maxHeight: .infinity)
            
            HStack {
                if let editor = vm.editor {
                    HStack(spacing: 25) {
                        if (editor.modified) {
                            Text("[modified]")
                                .padding(.leading, 5)
                        }
                        
                        Toolbar.Button {
                            
                        } label: {
                            Text(editor.fileName ?? "Untitled")
                        }
                        
                        Toolbar.Button {
                            
                        } label: {
                            Text("\(editor.line):\(editor.col)")
                        }
                        
                        Toolbar.Button {
                            
                        } label: {
                            Text(editor.indentation.description)
                        }
                        
                        Toolbar.Button {
                            
                        } label: {
                            Text(editor.encoding.description)
                        }
                        
                        Toolbar.Button {
                            
                        } label: {
                            Text(editor.endLine.description)
                        }
                        
                        Toolbar.Button {
                            
                        } label: {
                            Text(editor.type.description)
                        }
                    }
                } else {
                    Text("[inactive]")
                }
                
                Spacer()
            }
            .font(.body.monospaced())
            .frame(maxWidth: .infinity)
            .background(.ultraThickMaterial)
            .padding(.horizontal, 5)
            
            TextField("Type a command to start, e.g. \"save\"", text: $vm.commandLine, onCommit: vm.onCommandCommit)
                .font(.body.monospaced())
                .padding(.bottom, 5)
                .padding(.horizontal, 10)
                .textFieldStyle(.plain)
        }
#if os(macOS)
        .background(PromptApp.getBackgroundColor())
#endif
    }
}

#Preview {
    EditorView()
}

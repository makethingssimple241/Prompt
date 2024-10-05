//
//  EditorViewModel.swift
//  Prompt
//
//  Created by MakeThingsSimple on 30/9/2024.
//

import SwiftUI
import WebKit

extension WKWebView {
    
    struct Key {
        static let drawsBackground = "drawsBackground"
    }
    
    subscript(key: String) -> Any? {
        get {
            value(forKey: key)
        }
        set {
            setValue(newValue, forKey: key)
        }
    }
    
}

extension WKPreferences {
    
    struct Key {
        static let developerExtrasEnabled = "developerExtrasEnabled"
    }
    
    subscript(key: String) -> Any? {
        get {
            value(forKey: key)
        }
        set {
            setValue(newValue, forKey: key)
        }
    }
    
}

extension EditorView {
    
    class ViewModel: NSObject, ObservableObject, WKNavigationDelegate, WKUIDelegate {
        @Published var commandLine: String = ""
        
        var editor: Model? = nil
        var webView = WKWebView()
        let ollama = OllamaAPI(at: .init(string: "http://127.0.0.1:11434/api/chat")!)
        
        private let monacoEditorURL = Bundle.main.url(forResource: "MonacoEditor", withExtension: ".html")!
        
        override init() {
            super.init()
            
            // TODO: Remove
            self.editor = .init()
            self.editor?.modified = true
            
            #if os(macOS)
            self.webView[WKWebView.Key.drawsBackground] = false
            #endif
            
            self.webView.navigationDelegate = self
            self.webView.uiDelegate = self
            self.webView.configuration.preferences[WKPreferences.Key.developerExtrasEnabled] = true
        }
        
        func setFileName(fileName: String) {
            self.editor?.fileName = fileName
        }
        
        func onCommandCommit() {
            // TODO: Add access to functions implemented in main.py
            // TODO: natural language processing
            self.ollama.ask(model: .qwen2_0_5b, toRespondTo: self.commandLine) { response in
                if response.isEmpty  {
                    return
                }
                
                print(response)
                
                DispatchQueue.main.sync {
                    self.commandLine = response
                }
            }
        }
        
        func updateWebView() {
            let root = self.monacoEditorURL.deletingLastPathComponent()
            self.webView.loadFileURL(self.monacoEditorURL, allowingReadAccessTo: root)
        }
        
        private func getMonacoThemeName() -> String {
            let base = PromptApp.isDarkThemeEnabled() ? "vs-dark" : "vs-light"
            
            if let background = PromptApp.getWindowBackgroundColorAsHex() {
                let defineTheme = """
                    monaco.editor.defineTheme("prompt", {
                        base: "\(base)",
                        inherit: true,
                        rules: [{ background: "\(background)" }],
                        colors: {
                            "editor.background": "\(background)",
                        }
                    });
                """
                
                self.webView.evaluateJavaScript(defineTheme) { _, error in
                    if let error {
                        PromptApp.logger.warning("Failed to create Monaco editor: \(error)")
                    }
                }
                
                return "prompt"
            }
            
            return base
        }
        
        func createMonacoEditor() {
            guard let editor = self.editor else {
                return
            }
            
            let createEditor = """
                let editorElement = document.getElementById("monaco-editor");
            
                let editor = monaco.editor.create(editorElement, {
                    language: "\(editor.type.monacoEditorLanguage)",
                    theme: "\(self.getMonacoThemeName())",
                    value: ""
                });
            """
            
            self.webView.evaluateJavaScript(createEditor) { _, error in
                if let error {
                    PromptApp.logger.error("Failed to create Monaco editor: \(error)")
                }
            }
        }
        
        // WKNavigationDelegate
        
        func webView(_ webView: WKWebView, didFinish navigation: WKNavigation!) {
            guard webView == self.webView else {
                fatalError()
            }
            
            self.createMonacoEditor()
        }
    }
    
}

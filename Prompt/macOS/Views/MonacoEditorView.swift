//
//  MonacoEditorView.swift
//  Prompt
//
//  Created by MakeThingsSimple on 1/10/2024.
//

import SwiftUI
import WebKit

struct MonacoEditorView: NSViewRepresentable {
    var vm: EditorView.ViewModel
    
    func makeNSView(context: Context) -> WKWebView {
        return self.vm.webView
    }
    
    func updateNSView(_ view: WKWebView, context: Context) {
        self.vm.updateWebView()
    }
}

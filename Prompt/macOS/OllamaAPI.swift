//
//  Ollama.swift
//  Prompt
//
//  Created by MakeThingsSimple on 5/10/2024.
//

import Foundation

class OllamaAPI {
    let url: URL
    
    enum Model: CustomStringConvertible {
        case qwen2_0_5b
        
        var description: String {
            switch self {
            case .qwen2_0_5b:
                return "qwen2:0.5b"
            }
        }
    }
    
    init(at url: URL) {
        self.url = url
    }
    
    func ask(model: Model, toRespondTo message: String, withCompletionHandler completionHandler: @escaping (String) -> Void) {
        let body: [String:Any] = [
            "model": model.description,
            "messages": [
                [
                    "role": "user",
                    "content": message
                ]
            ]
        ]
        
        var request = URLRequest(url: self.url)
        request.httpBody = try! JSONSerialization.data(withJSONObject: body);
        request.httpMethod = "POST"
        
        var response = ""
        
        URLSession.shared
            .dataTask(with: request) { data, httpResponse, error in
                if error != nil {
                    print("Error: ", error)
                    return
                }
                
                guard let data else {
                    print("Error: data is nil")
                    return
                }
                
                let lines = String(data: data, encoding: .utf8)!
                    .components(separatedBy: .newlines)
                
                for line in lines {
                    let dictionary = try! JSONSerialization.jsonObject(with: line.data(using: .utf8)!) as! [String:Any]
                    
                    let message = dictionary["message"] as! [String:String]
                    response += message["content"]!
                    
                    if dictionary["done"]! as! Bool {
                        break
                    }
                }
                
                completionHandler(response)
            }
            .resume()
    }
}

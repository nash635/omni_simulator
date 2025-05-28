/*
 * omni_simulator - Network Routing Simulator
 * Copyright (c) 2025 nash635
 * Email: shaj24@mails.tsinghua.edu.cn
 * 
 * This software is licensed under the MIT License.
 * See the LICENSE file for more details.
 */

#ifndef TABLE_FORMATTER_H
#define TABLE_FORMATTER_H

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

class TableFormatter {
public:
    struct Column {
        std::string header;
        std::string unit;
        int width;
        int precision;
        bool isNumeric;
        
        Column(const std::string& h, const std::string& u, int w, int p = 3, bool numeric = true)
            : header(h), unit(u), width(w), precision(p), isNumeric(numeric) {}
    };
    
    TableFormatter(const std::string& title) : title_(title) {}
    
    void addColumn(const std::string& header, const std::string& unit, int width, int precision = 3, bool numeric = true) {
        columns_.emplace_back(header, unit, width, precision, numeric);
    }
    
    void addRow(const std::vector<std::string>& rowData) {
        if (rowData.size() != columns_.size()) {
            std::cerr << "Warning: Row data size mismatch" << std::endl;
            return;
        }
        
        std::vector<std::string> formattedRow;
        for (size_t i = 0; i < rowData.size(); ++i) {
            std::string cellData = rowData[i];
            
            if (columns_[i].precision > 0 && columns_[i].isNumeric) {
                try {
                    double value = std::stod(cellData);
                    std::ostringstream oss;
                    oss << std::fixed << std::setprecision(columns_[i].precision) << value;
                    cellData = oss.str();
                } catch (const std::exception& e) {
                    // Leave as original string if conversion fails
                }
            }
            
            formattedRow.push_back(cellData);
        }
        
        rows_.push_back(formattedRow);
    }
    
    void print() const {
        int totalWidth = 0;
        for (const auto& col : columns_) {
            totalWidth += col.width;
        }
        totalWidth += (columns_.size() - 1) * 3;
        
        std::cout << "\n" << std::string(totalWidth, '=') << std::endl;
        std::cout << std::setw(totalWidth) << std::left << title_ << std::endl;
        std::cout << std::string(totalWidth, '=') << std::endl;
        
        for (size_t i = 0; i < columns_.size(); ++i) {
            std::cout << std::setw(columns_[i].width) << std::left << columns_[i].header;
            if (i < columns_.size() - 1) std::cout << " | ";
        }
        std::cout << std::endl;
        
        for (size_t i = 0; i < columns_.size(); ++i) {
            std::cout << std::setw(columns_[i].width) << std::left << columns_[i].unit;
            if (i < columns_.size() - 1) std::cout << " | ";
        }
        std::cout << std::endl;
        
        std::cout << std::string(totalWidth, '-') << std::endl;
        
        for (const auto& row : rows_) {
            for (size_t i = 0; i < columns_.size() && i < row.size(); ++i) {
                if (columns_[i].isNumeric) {
                    try {
                        double value = std::stod(row[i]);
                        std::cout << std::fixed << std::setprecision(columns_[i].precision)
                                  << std::setw(columns_[i].width) << std::left << value;
                    } catch (const std::exception&) {
                        std::cout << std::setw(columns_[i].width) << std::left << row[i];
                    }
                } else {
                    std::cout << std::setw(columns_[i].width) << std::left << row[i];
                }
                if (i < columns_.size() - 1) std::cout << " | ";
            }
            std::cout << std::endl;
        }
        
        std::cout << std::string(totalWidth, '=') << std::endl;
    }
    
private:
    std::string title_;
    std::vector<Column> columns_;
    std::vector<std::vector<std::string>> rows_;
};

#endif // TABLE_FORMATTER_H